import socket

def receive_data(host, port):
    with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as s:
        s.connect((host, port))
        s.settimeout(50)  # Set a timeout of 50 seconds

        while True:
            try:
                data = s.recv(1024)
                if not data:
                    break
                print("Received:", data.decode())
            except socket.timeout:
                print("Socket timeout. No data received.")
                break
            except Exception as e:
                print(f"An error occurred: {e}")
                break

if __name__ == "__main__":
    esp32_ip = "192.168.4.1"  # Replace with your ESP32's IP address
    esp32_port = 250

    receive_data(esp32_ip, esp32_port)
