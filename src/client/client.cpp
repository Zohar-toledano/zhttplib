#include "client.hpp"

int getDefaultPort(HTTP_VERSION version)
{
	switch (version)
	{
	case HTTP_VERSION::HTTP_1_0:
	case HTTP_VERSION::HTTP_1_1:
	case HTTP_VERSION::HTTP_2_0:
		return 80;
	}
}
