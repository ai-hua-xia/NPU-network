from http.server import HTTPServer, BaseHTTPRequestHandler
import os
import urllib.parse
import mimetypes

class SimpleHandler(BaseHTTPRequestHandler):
    def do_GET(self):
        # 如果请求路径是根目录 /，就映射到 index.html
        if self.path == "/":
            self.path = "/index.html"

        # 处理动态请求，例如 /add?a=5&b=3
        if self.path.startswith("/add"):
            self.handle_addition()

        else:
            # 处理静态文件请求
            try:
                file_path = self.path.strip("/")
                with open(file_path, 'rb') as file:
                    content = file.read()
                    mime_type, _ = mimetypes.guess_type(file_path)
                    self.respond(200, content, mime_type)
            except FileNotFoundError:
                self.respond(404, "File not found".encode())

    def handle_addition(self):
        # 解析查询字符串
        query = urllib.parse.urlparse(self.path).query
        params = urllib.parse.parse_qs(query)
        try:
            a = int(params.get('a', [0])[0])
            b = int(params.get('b', [0])[0])
            result = a + b
            response = f"Result: {result}"
            self.respond(200, response.encode())
        except ValueError:
            self.respond(400, "Invalid input. Please provide two numbers as 'a' and 'b'.".encode())

    def respond(self, status, content, content_type="text/html"):
        self.send_response(status)
        self.send_header("Content-type", content_type)
        self.send_header("Content-Length", str(len(content)))
        self.end_headers()
        self.wfile.write(content)

    def do_HEAD(self):
        # 处理 HEAD 请求
        if self.path == "/":
            self.path = "/index.html"
        
        if self.path.startswith("/add"):
            # 处理动态请求
            query = urllib.parse.urlparse(self.path).query
            params = urllib.parse.parse_qs(query)
            try:
                a = int(params.get('a', [0])[0])
                b = int(params.get('b', [0])[0])
                result = a + b
                response = f"Result: {result}"
                self.send_response(200)
                self.send_header("Content-type", "text/plain")
                self.send_header("Content-Length", str(len(response.encode())))
                self.end_headers()
            except ValueError:
                self.send_response(400)
                self.end_headers()
        else:
            try:
                # 读取文件内容
                with open(self.path.strip("/"), 'rb') as file:
                    content = file.read()
                    mime_type, _ = mimetypes.guess_type(self.path.strip("/"))
                    self.send_response(200)
                    self.send_header("Content-type", mime_type)
                    self.send_header("Content-Length", str(len(content)))
                    self.end_headers()
            except FileNotFoundError:
                self.send_response(404)
                self.end_headers()

    def do_DELETE(self):
        # 处理 DELETE 请求
        file_path = self.path.strip("/")
        if os.path.exists(file_path):
            try:
                os.remove(file_path)
                self.respond(200, f"File {file_path} deleted".encode())
            except Exception as e:
                self.respond(500, f"Error deleting file: {str(e)}".encode())
        else:
            self.respond(404, "File not found".encode())

def start_server():
    server = HTTPServer(("localhost", 8000), SimpleHandler)
    print("Server started at http://localhost:8000")
    server.serve_forever()

if __name__ == "__main__":
    start_server()