# WEBSERVER with SSL support
# Create certificate files ca_key.pem and ca_cert.pem and they should be in the same folder

# Output when client connects:
# Web Server at => 192.168.1.100:443
# 192.168.1.22 - - [12/Feb/2022 02:32:56] "GET /default.html HTTP/1.1" 200 -
import http.server
import ssl

HOST = '192.168.1.128'
PORT = 443
Handler = http.server.SimpleHTTPRequestHandler 
with http.server.HTTPServer((HOST, PORT), Handler) as httpd:
    print("Web Server listening at => " + HOST + ":" + str(PORT))
    sslcontext = ssl.SSLContext(ssl.PROTOCOL_TLS_SERVER)
    sslcontext.load_cert_chain(keyfile="server_certs/ca_key.pem", certfile="server_certs/ca_cert.pem")
    httpd.socket = sslcontext.wrap_socket(httpd.socket, server_side=True)
    httpd.serve_forever()
