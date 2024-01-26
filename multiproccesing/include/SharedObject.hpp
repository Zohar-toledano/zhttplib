#pragma once

#include <windows.h>
#include <iostream>
#include <fstream>

void dumpToFile(const char *filename, const char *data, size_t size)
{
	std::ofstream file(filename, std::ios::out | std::ios::binary);

	if (!file)
	{
		std::cerr << "Cannot open file: " << filename << std::endl;
		return;
	}

	file.write(data, size);
	file.close();
}

template <typename T, typename ExtraData>
struct SharedQueue
{
private:
	int front;
	int rear;
	int maxSize; // Maximum size of the queue

public:
	ExtraData data;
	SharedQueue() {}

	void close()
	{
		UnmapViewOfFile(this);
	}

	int getTotalSize()
	{
		return sizeof(SharedQueue<T, ExtraData>) + maxSize * sizeof(T);
	}

	inline T *getStartOfArr(SharedQueue<T, ExtraData> **start)
	{
		return reinterpret_cast<T *>(*start + 1); // Point to the memory right after the struct
	}

	bool push(T &value, SharedQueue<T, ExtraData> **start, HANDLE *hMutex)
	{
		// rear = push_counter

		DWORD dwWaitResult = WaitForSingleObject(*hMutex, INFINITE);
		if (dwWaitResult != WAIT_OBJECT_0)
		{
			std::cerr << "Wait error push: " << GetLastError() << std::endl;
			return false;
		}

		if ((rear + 1) % maxSize == front)
		{
			ReleaseMutex(*hMutex);
			return false; // Queue is full
		}
		auto data = getStartOfArr(start);
		memcpy(&data[rear], &value, sizeof(T));

		rear = (rear + 1) % maxSize;
		ReleaseMutex(*hMutex);
		return true;

		// return true;
	}

	bool pop(T &value, SharedQueue<T, ExtraData> **start, HANDLE *hMutex)
	{
		DWORD dwWaitResult = WaitForSingleObject(*hMutex, INFINITE);
		if (dwWaitResult != WAIT_OBJECT_0)
		{
			std::cerr << "Wait error: " << GetLastError() << std::endl;
			return false;
		}

		if (front == rear)
		{
			ReleaseMutex(*hMutex);
			return false; // Queue is empty
		}
		auto data = getStartOfArr(start);
		memcpy(&value, &data[front], sizeof(T));

		front = (front + 1) % maxSize;

		if (front == rear)
			front = rear = 0;

		ReleaseMutex(*hMutex);
		return true;
	}

	static int getTotalSize(int maxSize)
	{
		return sizeof(SharedQueue<T, ExtraData>) + maxSize * sizeof(T);
	}

	inline unsigned int length()
	{
		return front - rear;
	}

	static bool create(int maxSize, const char *name, SharedQueue<T, ExtraData> **res, HANDLE *hMapFile)
	{
		int totalSize = SharedQueue<T, ExtraData>::getTotalSize(maxSize);
		*hMapFile = CreateFileMappingA(
			INVALID_HANDLE_VALUE, // Use paging file
			NULL,				  // Default security
			PAGE_READWRITE,		  // Read/write access
			0,					  // Maximum object size (high-order DWORD)
			totalSize,			  // Maximum object size (low-order DWORD)
			name);				  // Name of mapping object
		if (*hMapFile == NULL)
		{
			std::cerr << "Could not create file mapping object: " << GetLastError() << std::endl;
			return false;
		}
		if (!getMemoryPointer(maxSize, name, res, hMapFile))
		{
			return false;
		}

		ZeroMemory(*res, totalSize);

		(*res)->front = 0;
		(*res)->rear = 0;
		(*res)->maxSize = maxSize;
		// (*res)->data = reinterpret_cast<T *>(*res + 1); // Point to the memory right after the struct
		return true;
	}

	static bool connect(int maxSize, const char *name, SharedQueue<T, ExtraData> **res, HANDLE *hMapFile)
	{
		*hMapFile = OpenFileMappingA(
			FILE_MAP_ALL_ACCESS, // Read/write permission
			FALSE,				 // Do not inherit the name
			name);				 // Name of mapping object

		if (*hMapFile == NULL)
		{
			std::cerr << "Could not open file mapping object: " << GetLastError() << std::endl;
			return false;
		}

		return getMemoryPointer(maxSize, name, res, hMapFile);
	}

	/**
	 * Retrieves a memory pointer given the maximum size, name, and handles.
	 *
	 * @param maxSize the maximum size of the memory pointer
	 * @param name the name of the memory pointer
	 * @param res the result of the memory pointer
	 * @param hMapFile the handle to the map file
	 *
	 * @return true if the memory pointer was retrieved successfully, false otherwise
	 *
	 * @throws None
	 */
	static bool getMemoryPointer(int maxSize, const char *name, SharedQueue<T, ExtraData> **res, HANDLE *hMapFile)
	{
		int totalSize = SharedQueue<T, ExtraData>::getTotalSize(maxSize);
		*res = reinterpret_cast<SharedQueue<T, ExtraData> *>(MapViewOfFile(
			*hMapFile,			 // Handle to map object
			FILE_MAP_ALL_ACCESS, // Read/write permission
			0,
			0,
			totalSize));
		if (*res == NULL)
		{
			std::cerr << "Could not map view of file: " << GetLastError() << std::endl;
			CloseHandle(*hMapFile);
			return false;
		}
		return true;
	}
};

template <typename T, typename ExtraData>
struct SharedObject
{
private:
	SharedQueue<T, ExtraData> *queue;
	HANDLE hMapFile;
	HANDLE hMutex;
	bool creator;

public:
	void setData(ExtraData data)
	{
		queue->data = data;
	}
	ExtraData getData()
	{
		return queue->data;
	}

	SharedObject() = default;
	/**
	 * Constructs a SharedQueueWrapper object with the given maximum size, name, and an optional create flag.
	 *
	 * @param maxSize The maximum size of the shared queue.
	 * @param name The name of the shared queue.
	 * @param create Flag indicating whether to create a new shared queue or connect to an existing one. Defaults to false.
	 *
	 * @throws None
	 */
	SharedObject(int maxSize, const char *name, bool create = false) : creator(create)
	{
		if (create)
			SharedQueue<T, ExtraData>::create(maxSize, name, &queue, &hMapFile);

		else
			SharedQueue<T, ExtraData>::connect(maxSize, name, &queue, &hMapFile);
		

		SECURITY_ATTRIBUTES sa;
		sa.nLength = sizeof(sa);
		sa.lpSecurityDescriptor = NULL; // Uses the default security descriptor if NULL
		sa.bInheritHandle = TRUE;		// The handle is inheritable
		const char *mutex_name = (std::string(name) + "_mutex").c_str();
		if ((hMutex = CreateMutexA(&sa, FALSE, mutex_name)) == NULL)
		{
			// raise an exception!
			std::cerr << "error creating mutex\n";
		}
	}

	SharedObject(int maxSize, const char *name, bool create, ExtraData data)
		: SharedObject(maxSize, name, create)
	{
		queue->data = data;
	}

	SharedObject &operator=(const SharedObject &other)
	{
		if (this != &other)
		{
			return (SharedObject &)other;
		}
		return *this;
	}

	bool push(T &value)
	{
		return queue->push(value, &queue, &hMutex);
	}

	bool push(const T &value)
	{
		return push((T &)value);
	}

	bool pop(T &value)
	{
		return queue->pop(value, &queue, &hMutex);
	}

	inline unsigned int length()
	{
		return queue->length();
	}

	~SharedObject()
	{
		if (creator)
			queue->close();
		CloseHandle(hMapFile);
	}
};
