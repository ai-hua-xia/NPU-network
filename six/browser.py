# import requests
# from tkinter import Tk, Label, Entry, Button, Text, END, Canvas
# import socket
# from bs4 import BeautifulSoup
# from PIL import Image, ImageTk
# import io
# from urllib.parse import urljoin

# def clear_canvas():
#     # 清除 Canvas 中的所有图像
#     for widget in app.grid_slaves():
#         if isinstance(widget, Canvas):
#             widget.destroy()

# def get_ip_from_hosts(hostname):
#     hosts_path = r"C:\Windows\System32\drivers\etc\hosts"
#     with open(hosts_path, 'r') as file:
#         for line in file:
#             if hostname.lower() in line.lower():
#                 parts = line.split()
#                 if len(parts) >= 2:
#                     return parts[0]
#     return None

# def handle_response(response):
#     status_code = response.status_code
#     if 200 <= status_code < 300:
#         return f"Status Code: {status_code}\n{response.text}"
#     elif 300 <= status_code < 400:
#         return f"Status Code: {status_code}\nRedirection: The requested URL has been redirected to {response.headers.get('Location', 'another location')}."
#     elif 400 <= status_code < 500:
#         if status_code == 400:
#             return f"Status Code: {status_code}\nError 400: Bad Request - The server could not understand the request."
#         elif status_code == 403:
#             return f"Status Code: {status_code}\nError 403: Forbidden - The server is refusing to fulfill the request."
#         elif status_code == 404:
#             return f"Status Code: {status_code}\nError 404: Not Found - The server has not found anything matching the Request-URI."
#         else:
#             return f"Status Code: {status_code}\nClient Error {status_code}: {response.reason}"
#     elif 500 <= status_code < 600:
#         if status_code == 501:
#             return f"Status Code: {status_code}\nError 501: Not Implemented - The server does not support the functionality required to fulfill the request."
#         elif status_code == 505:
#             return f"Status Code: {status_code}\nError 505: HTTP Version Not Supported - The server does not support the HTTP protocol version that was used in the request."
#         else:
#             return f"Status Code: {status_code}\nServer Error {status_code}: {response.reason}"
#     else:
#         return f"Status Code: {status_code}\nUnexpected status code {status_code}: {response.reason}"

# def send_request(method):
#     url = url_entry.get().lower()
#     hostname = "www.baidu.com"
#     if hostname in url:
#         ip_address = get_ip_from_hosts(hostname)
#         if not ip_address:
#             display_area.delete("1.0", END)
#             display_area.insert(END, f"Error: Unable to resolve {hostname} from hosts file.\n")
#             return

#         try:
#             # 建立TCP连接
#             port = 80
#             with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as s:
#                 s.connect((ip_address, port))
#                 request = f"{method} / HTTP/1.1\r\nHost: {hostname}\r\nConnection: close\r\n\r\n"
#                 s.sendall(request.encode())

#                 # 接收服务器返回应答
#                 response = s.recv(4096).decode()
#                 display_area.delete("1.0", END)
#                 display_area.insert(END, response)
#         except Exception as e:
#             display_area.delete("1.0", END)
#             display_area.insert(END, f"Error: Unable to connect to {hostname}\n{str(e)}")
#     else:
#         try:
#             # 每次请求前先清空 Canvas 中的内容
#             clear_canvas()
            
#             if method == "GET":
#                 # 动态网页判断：以 /add 开头的 URL
#                 if url.startswith("http://localhost:8000/add"):
#                     # 处理动态网页：例如 http://localhost:8000/add?a=5&b=3
#                     try:
#                         response = requests.get(url)
#                         display_area.delete("1.0", END)
#                         if response.status_code == 200:
#                             display_area.insert(END, f"Result from dynamic page: {response.text}\n")
#                         else:
#                             display_area.insert(END, handle_response(response))
#                     except requests.exceptions.RequestException as e:
#                         display_area.delete("1.0", END)
#                         display_area.insert(END, f"Error: Unable to connect to {url}\n{str(e)}")
#                 # 静态网页判断：以 .html 或 .htm 结尾的 URL
#                 elif url.endswith(".html") or url.endswith(".htm"):
#                     response = requests.get(url)
#                     display_area.delete("1.0", END)
#                     if response.status_code == 200:
#                         # 显示响应头部信息
#                         display_area.insert(END, f"HTTP/1.0 {response.status_code} OK\n")
#                         for key, value in response.headers.items():
#                             display_area.insert(END, f"{key}: {value}\n")
#                         display_area.insert(END, "\n")

#                         # 解析并显示 <body> 部分的文本信息
#                         soup = BeautifulSoup(response.text, 'html.parser')
#                         body = soup.find('body')
#                         if body:
#                             for element in body.find_all(['h1', 'p']):
#                                 display_area.insert(END, f"{element.text}\n")

#                             # 处理图片
#                             img_element = body.find('img')
#                             if img_element:
#                                 img_url = urljoin(url, img_element['src'])  # 将相对路径转换为绝对路径
#                                 img_response = requests.get(img_url)
#                                 img_data = img_response.content
#                                 img = Image.open(io.BytesIO(img_data))
#                                 img = ImageTk.PhotoImage(img)

#                                 # 使用 Canvas 显示图片，确保对图片对象的引用保持
#                                 canvas = Canvas(app, width=img.width(), height=img.height())
#                                 canvas.grid(row=4, column=0, columnspan=3)
#                                 canvas.create_image(0, 0, anchor='nw', image=img)

#                                 # 将图像保存在 Canvas 对象中，防止垃圾回收
#                                 canvas.image = img
#                         else:
#                             display_area.insert(END, "No <body> section found.")
#                     else:
#                         display_area.insert(END, handle_response(response))
#             elif method == "HEAD":
#                 response = requests.head(url)
#                 display_area.delete("1.0", END)
#                 # 格式化响应头部信息
#                 formatted_headers = f"HEAD {url} HTTP/1.0\n"
#                 formatted_headers += f"Host: {response.request.headers.get('Host', url.split('/')[2])}\n"
#                 formatted_headers += f"Connection: {response.request.headers.get('Connection', 'close')}\n"
#                 formatted_headers += f"User-agent: {response.request.headers.get('User-Agent', 'WIN/4.0')}\n"
#                 formatted_headers += f"Accept: {response.request.headers.get('Accept', 'text/html, image/gif, image/jpeg')}\n"
#                 formatted_headers += f"Accept-language: {response.request.headers.get('Accept-Language', 'cn')}\n"
#                 display_area.insert(END, formatted_headers)
#             elif method == "DELETE":
#                 response = requests.delete(url)
#                 display_area.delete("1.0", END)
#                 # 格式化响应头部信息
#                 formatted_headers = f"DELETE {url} HTTP/1.0\n"
#                 formatted_headers += f"Host: {response.request.headers.get('Host', url.split('/')[2])}\n"
#                 formatted_headers += f"Connection: {response.request.headers.get('Connection', 'close')}\n"
#                 formatted_headers += f"User-agent: {response.request.headers.get('User-Agent', 'WIN/4.0')}\n"
#                 formatted_headers += f"Accept: {response.request.headers.get('Accept', 'text/html, image/gif, image/jpeg')}\n"
#                 formatted_headers += f"Accept-language: {response.request.headers.get('Accept-Language', 'cn')}\n"
#                 display_area.insert(END, formatted_headers)
#                 display_area.insert(END, handle_response(response))
#         except requests.exceptions.RequestException as e:
#             display_area.delete("1.0", END)
#             display_area.insert(END, f"Error: Unable to connect to {url}\n{str(e)}")

# # GUI
# app = Tk()
# app.title("Simple Browser")

# Label(app, text="URL:").grid(row=0, column=0)
# url_entry = Entry(app, width=50)
# url_entry.grid(row=0, column=1)

# # 创建GET按钮
# get_button = Button(app, text="GET", command=lambda: send_request("GET"))
# get_button.grid(row=1, column=0)

# # 创建HEAD按钮
# head_button = Button(app, text="HEAD", command=lambda: send_request("HEAD"))
# head_button.grid(row=1, column=1)

# # 创建DELETE按钮
# delete_button = Button(app, text="DELETE", command=lambda: send_request("DELETE"))
# delete_button.grid(row=1, column=2)

# display_area = Text(app, wrap="word", height=20, width=80)
# display_area.grid(row=3, column=0, columnspan=3)

# app.mainloop()




#-------------------------------------------------------------------
# import sys
# from PyQt5.QtWidgets import QApplication, QMainWindow, QVBoxLayout, QWidget, QPushButton, QLineEdit
# from PyQt5.QtWebEngineWidgets import QWebEngineView
# from PyQt5.QtCore import QUrl

# class BrowserApp(QMainWindow):
#     def __init__(self):
#         super().__init__()

#         self.setWindowTitle("PyQt5 Browser")

#         # 创建主窗口的布局
#         self.central_widget = QWidget()
#         self.setCentralWidget(self.central_widget)
#         self.layout = QVBoxLayout(self.central_widget)

#         # 创建输入框和按钮
#         self.url_input = QLineEdit(self)
#         self.url_input.setPlaceholderText("Enter URL (e.g., http://example.com)")
#         self.layout.addWidget(self.url_input)

#         self.fetch_button = QPushButton("Fetch Page", self)
#         self.fetch_button.clicked.connect(self.load_url)
#         self.layout.addWidget(self.fetch_button)

#         # 创建 WebEngineView，用于显示网页内容
#         self.web_view = QWebEngineView(self)
#         self.layout.addWidget(self.web_view)

#     def load_url(self):
#         url = self.url_input.text().strip()
#         if not url.startswith("http://") and not url.startswith("https://"):
#             url = "http://" + url

#         self.web_view.setUrl(QUrl(url))  # 直接加载URL

# def main():
#     app = QApplication(sys.argv)
#     browser_app = BrowserApp()
#     browser_app.show()
#     sys.exit(app.exec_())

# if __name__ == "__main__":
#     main()



#-------------------------------------------------------------
# import requests
# from tkinter import Tk, Label, Entry, Button, Text, END, Canvas
# import socket
# from bs4 import BeautifulSoup
# from PIL import Image, ImageTk
# import io
# from urllib.parse import urljoin
# import sys
# from PyQt5.QtWidgets import QApplication, QMainWindow, QVBoxLayout, QWidget, QPushButton, QLineEdit
# from PyQt5.QtWebEngineWidgets import QWebEngineView
# from PyQt5.QtCore import QUrl

# def clear_canvas():
#     # 清除 Canvas 中的所有图像
#     for widget in app.grid_slaves():
#         if isinstance(widget, Canvas):
#             widget.destroy()

# def get_ip_from_hosts(hostname):
#     hosts_path = r"C:\Windows\System32\drivers\etc\hosts"
#     with open(hosts_path, 'r') as file:
#         for line in file:
#             if hostname.lower() in line.lower():
#                 parts = line.split()
#                 if len(parts) >= 2:
#                     return parts[0]
#     return None

# def handle_response(response):
#     status_code = response.status_code
#     if 200 <= status_code < 300:
#         return f"Status Code: {status_code}\n{response.text}"
#     elif 300 <= status_code < 400:
#         return f"Status Code: {status_code}\nRedirection: The requested URL has been redirected to {response.headers.get('Location', 'another location')}."
#     elif 400 <= status_code < 500:
#         if status_code == 400:
#             return f"Status Code: {status_code}\nError 400: Bad Request - The server could not understand the request."
#         elif status_code == 403:
#             return f"Status Code: {status_code}\nError 403: Forbidden - The server is refusing to fulfill the request."
#         elif status_code == 404:
#             return f"Status Code: {status_code}\nError 404: Not Found - The server has not found anything matching the Request-URI."
#         else:
#             return f"Status Code: {status_code}\nClient Error {status_code}: {response.reason}"
#     elif 500 <= status_code < 600:
#         if status_code == 501:
#             return f"Status Code: {status_code}\nError 501: Not Implemented - The server does not support the functionality required to fulfill the request."
#         elif status_code == 505:
#             return f"Status Code: {status_code}\nError 505: HTTP Version Not Supported - The server does not support the HTTP protocol version that was used in the request."
#         else:
#             return f"Status Code: {status_code}\nServer Error {status_code}: {response.reason}"
#     else:
#         return f"Status Code: {status_code}\nUnexpected status code {status_code}: {response.reason}"

# def send_request(method):
#     url = url_entry.get().lower()
#     hostname = "www.baidu.com"
#     if hostname in url:
#         ip_address = get_ip_from_hosts(hostname)
#         if not ip_address:
#             display_area.delete("1.0", END)
#             display_area.insert(END, f"Error: Unable to resolve {hostname} from hosts file.\n")
#             return

#         try:
#             # 建立TCP连接
#             port = 80
#             with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as s:
#                 s.connect((ip_address, port))
#                 request = f"{method} / HTTP/1.1\r\nHost: {hostname}\r\nConnection: close\r\n\r\n"
#                 s.sendall(request.encode())

#                 # 接收服务器返回应答
#                 response = s.recv(4096).decode()
#                 display_area.delete("1.0", END)
#                 display_area.insert(END, response)
#         except Exception as e:
#             display_area.delete("1.0", END)
#             display_area.insert(END, f"Error: Unable to connect to {hostname}\n{str(e)}")
#     else:
#         try:
#             # 每次请求前先清空 Canvas 中的内容
#             clear_canvas()
            
#             if method == "GET":
#                 # 使用 PyQt5 显示网页内容
#                 app = QApplication(sys.argv)
#                 browser_app = BrowserApp()
#                 browser_app.url_input.setText(url)
#                 browser_app.load_url()
#                 browser_app.show()
#                 sys.exit(app.exec_())
#             elif method == "HEAD":
#                 response = requests.head(url)
#                 display_area.delete("1.0", END)
#                 # 格式化响应头部信息
#                 formatted_headers = f"HEAD {url} HTTP/1.0\n"
#                 formatted_headers += f"Host: {response.request.headers.get('Host', url.split('/')[2])}\n"
#                 formatted_headers += f"Connection: {response.request.headers.get('Connection', 'close')}\n"
#                 formatted_headers += f"User-agent: {response.request.headers.get('User-Agent', 'WIN/4.0')}\n"
#                 formatted_headers += f"Accept: {response.request.headers.get('Accept', 'text/html, image/gif, image/jpeg')}\n"
#                 formatted_headers += f"Accept-language: {response.request.headers.get('Accept-Language', 'cn')}\n"
#                 display_area.insert(END, formatted_headers)
#             elif method == "DELETE":
#                 response = requests.delete(url)
#                 display_area.delete("1.0", END)
#                 # 格式化响应头部信息
#                 formatted_headers = f"DELETE {url} HTTP/1.0\n"
#                 formatted_headers += f"Host: {response.request.headers.get('Host', url.split('/')[2])}\n"
#                 formatted_headers += f"Connection: {response.request.headers.get('Connection', 'close')}\n"
#                 formatted_headers += f"User-agent: {response.request.headers.get('User-Agent', 'WIN/4.0')}\n"
#                 formatted_headers += f"Accept: {response.request.headers.get('Accept', 'text/html, image/gif, image/jpeg')}\n"
#                 formatted_headers += f"Accept-language: {response.request.headers.get('Accept-Language', 'cn')}\n"
#                 display_area.insert(END, formatted_headers)
#                 display_area.insert(END, handle_response(response))
#         except requests.exceptions.RequestException as e:
#             display_area.delete("1.0", END)
#             display_area.insert(END, f"Error: Unable to connect to {url}\n{str(e)}")

# class BrowserApp(QMainWindow):
#     def __init__(self):
#         super().__init__()

#         self.setWindowTitle("PyQt5 Browser")

#         # 创建主窗口的布局
#         self.central_widget = QWidget()
#         self.setCentralWidget(self.central_widget)
#         self.layout = QVBoxLayout(self.central_widget)

#         # 创建输入框和按钮
#         self.url_input = QLineEdit(self)
#         self.url_input.setPlaceholderText("Enter URL (e.g., http://example.com)")
#         self.layout.addWidget(self.url_input)

#         self.fetch_button = QPushButton("Fetch Page", self)
#         self.fetch_button.clicked.connect(self.load_url)
#         self.layout.addWidget(self.fetch_button)

#         # 创建 WebEngineView，用于显示网页内容
#         self.web_view = QWebEngineView(self)
#         self.layout.addWidget(self.web_view)

#     def load_url(self):
#         url = self.url_input.text().strip()
#         if not url.startswith("http://") and not url.startswith("https://"):
#             url = "http://" + url

#         self.web_view.setUrl(QUrl(url))  # 直接加载URL

# # GUI
# app = Tk()
# app.title("Simple Browser")

# Label(app, text="URL:").grid(row=0, column=0)
# url_entry = Entry(app, width=50)
# url_entry.grid(row=0, column=1)

# # 创建GET按钮
# get_button = Button(app, text="GET", command=lambda: send_request("GET"))
# get_button.grid(row=1, column=0)

# # 创建HEAD按钮
# head_button = Button(app, text="HEAD", command=lambda: send_request("HEAD"))
# head_button.grid(row=1, column=1)

# # 创建DELETE按钮
# delete_button = Button(app, text="DELETE", command=lambda: send_request("DELETE"))
# delete_button.grid(row=1, column=2)

# display_area = Text(app, wrap="word", height=20, width=80)
# display_area.grid(row=3, column=0, columnspan=3)

# app.mainloop()



import requests
from tkinter import Tk, Label, Entry, Button, Text, END, Canvas
import socket
from bs4 import BeautifulSoup
from PIL import Image, ImageTk
import io
from urllib.parse import urljoin
import webbrowser  # Import webbrowser module to open URLs in the default browser

def clear_canvas():
    # 清除 Canvas 中的所有图像
    for widget in app.grid_slaves():
        if isinstance(widget, Canvas):
            widget.destroy()

def get_ip_from_hosts(hostname):
    hosts_path = r"C:\Windows\System32\drivers\etc\hosts"
    with open(hosts_path, 'r') as file:
        for line in file:
            if hostname.lower() in line.lower():
                parts = line.split()
                if len(parts) >= 2:
                    return parts[0]
    return None

def handle_response(response):
    status_code = response.status_code
    if 200 <= status_code < 300:
        return f"Status Code: {status_code}\n{response.text}"
    elif 300 <= status_code < 400:
        return f"Status Code: {status_code}\nRedirection: The requested URL has been redirected to {response.headers.get('Location', 'another location')}."
    elif 400 <= status_code < 500:
        if status_code == 400:
            return f"Status Code: {status_code}\nError 400: Bad Request - The server could not understand the request."
        elif status_code == 403:
            return f"Status Code: {status_code}\nError 403: Forbidden - The server is refusing to fulfill the request."
        elif status_code == 404:
            return f"Status Code: {status_code}\nError 404: Not Found - The server has not found anything matching the Request-URI."
        else:
            return f"Status Code: {status_code}\nClient Error {status_code}: {response.reason}"
    elif 500 <= status_code < 600:
        if status_code == 501:
            return f"Status Code: {status_code}\nError 501: Not Implemented - The server does not support the functionality required to fulfill the request."
        elif status_code == 505:
            return f"Status Code: {status_code}\nError 505: HTTP Version Not Supported - The server does not support the HTTP protocol version that was used in the request."
        else:
            return f"Status Code: {status_code}\nServer Error {status_code}: {response.reason}"
    else:
        return f"Status Code: {status_code}\nUnexpected status code {status_code}: {response.reason}"

def send_request(method):
    url = url_entry.get().lower()
    hostname = "www.baidu.com"
    if hostname in url:
        ip_address = get_ip_from_hosts(hostname)
        if not ip_address:
            display_area.delete("1.0", END)
            display_area.insert(END, f"Error: Unable to resolve {hostname} from hosts file.\n")
            return

        try:
            # 建立TCP连接
            port = 80
            with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as s:
                s.connect((ip_address, port))
                request = f"{method} / HTTP/1.1\r\nHost: {hostname}\r\nConnection: close\r\n\r\n"
                s.sendall(request.encode())

                # 接收服务器返回应答
                response = s.recv(4096).decode()
                display_area.delete("1.0", END)
                display_area.insert(END, response)
        except Exception as e:
            display_area.delete("1.0", END)
            display_area.insert(END, f"Error: Unable to connect to {hostname}\n{str(e)}")
    else:
        try:
            # 每次请求前先清空 Canvas 中的内容
            clear_canvas()

            # Check if the URL is a static page (ends with .html or .htm)
            if method == "GET" and (url.endswith(".html") or url.endswith(".htm")):
                # Open the static page URL in the default web browser
                webbrowser.open(url)

                # Get the response and show details in the display area
                response = requests.get(url)
                display_area.delete("1.0", END)
                if response.status_code == 200:
                    # Format and display the status code, headers, and other information
                    display_area.insert(END, f"Status Code: {response.status_code}\n")
                    for key, value in response.headers.items():
                        display_area.insert(END, f"{key}: {value}\n")
                    display_area.insert(END, "\n")
                    # Optionally, you could also display the body content (e.g., the first 500 characters)
                    display_area.insert(END, response.text[:500])  # Displaying a preview of the body content
                else:
                    display_area.insert(END, handle_response(response))

            elif method == "GET" and url.startswith("http://localhost:8000/add"):
                # Handle dynamic page request (keep the current behavior)
                try:
                    response = requests.get(url)
                    display_area.delete("1.0", END)
                    if response.status_code == 200:
                        display_area.insert(END, f"Result from dynamic page: {response.text}\n")
                    else:
                        display_area.insert(END, handle_response(response))
                except requests.exceptions.RequestException as e:
                    display_area.delete("1.0", END)
                    display_area.insert(END, f"Error: Unable to connect to {url}\n{str(e)}")
            elif method == "HEAD":
                response = requests.head(url)
                display_area.delete("1.0", END)
                # 格式化响应头部信息
                formatted_headers = f"HEAD {url} HTTP/1.0\n"
                formatted_headers += f"Host: {response.request.headers.get('Host', url.split('/')[2])}\n"
                formatted_headers += f"Connection: {response.request.headers.get('Connection', 'close')}\n"
                formatted_headers += f"User-agent: {response.request.headers.get('User-Agent', 'WIN/4.0')}\n"
                formatted_headers += f"Accept: {response.request.headers.get('Accept', 'text/html, image/gif, image/jpeg')}\n"
                formatted_headers += f"Accept-language: {response.request.headers.get('Accept-Language', 'cn')}\n"
                display_area.insert(END, formatted_headers)
            elif method == "DELETE":
                response = requests.delete(url)
                display_area.delete("1.0", END)
                # 格式化响应头部信息
                formatted_headers = f"DELETE {url} HTTP/1.0\n"
                formatted_headers += f"Host: {response.request.headers.get('Host', url.split('/')[2])}\n"
                formatted_headers += f"Connection: {response.request.headers.get('Connection', 'close')}\n"
                formatted_headers += f"User-agent: {response.request.headers.get('User-Agent', 'WIN/4.0')}\n"
                formatted_headers += f"Accept: {response.request.headers.get('Accept', 'text/html, image/gif, image/jpeg')}\n"
                formatted_headers += f"Accept-language: {response.request.headers.get('Accept-Language', 'cn')}\n"
                display_area.insert(END, formatted_headers)
                display_area.insert(END, handle_response(response))
        except requests.exceptions.RequestException as e:
            display_area.delete("1.0", END)
            display_area.insert(END, f"Error: Unable to connect to {url}\n{str(e)}")

# GUI
app = Tk()
app.title("Simple Browser")

Label(app, text="URL:").grid(row=0, column=0)
url_entry = Entry(app, width=50)
url_entry.grid(row=0, column=1)

# 创建GET按钮
get_button = Button(app, text="GET", command=lambda: send_request("GET"))
get_button.grid(row=1, column=0)

# 创建HEAD按钮
head_button = Button(app, text="HEAD", command=lambda: send_request("HEAD"))
head_button.grid(row=1, column=1)

# 创建DELETE按钮
delete_button = Button(app, text="DELETE", command=lambda: send_request("DELETE"))
delete_button.grid(row=1, column=2)

display_area = Text(app, wrap="word", height=20, width=80)
display_area.grid(row=3, column=0, columnspan=3)

app.mainloop()
