import socket
# text = """POST /cgi-bin/process.cgi HTTP/1.1
# User-Agent: Mozilla/4.0 (compatible; MSIE5.01; Windows NT)
# Host: www.tutorialspoint.com
# Content-Type: application/x-www-form-urlencoded
# Content-Length: length
# Accept-Language: en-us
# Accept-Encoding: gzip, deflate
# Connection: Keep-Alive

# licenseID=string&content=string&/paramsXML=string"""+("text"*3000)
s = socket.socket(socket.AF_INET,socket.SOCK_STREAM)
# s.connect(("127.0.0.1",8080))

# # s.send(("1"*31+"2").encode())
# s.send(" ".encode())
# print(s.recv(1024).decode())
# input()
# s.close()
# body = ("hi"*80000000)+"bbbbbbbbbbbbbb"
# text = f"""HTTP/1.1 200 OK
# Server: test server
# connection: keep-alive

# """+body

s.bind(("127.0.0.1",8080))
s.listen()
c,ca = s.accept()
print(c.recv(1000))
# c.send("dsda")
# input()


# import flask
# app = flask.Flask(__name__)

# @app.route("/test")
# def tet():
#     return "hi"

# app.run("127.0.0.1",8080)
# import requests

# print(requests.post("http://localhost:8080",data="5"+("hi"*900)+"5"))

