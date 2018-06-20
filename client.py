from coapthon.client.helperclient import HelperClient

host = "192.168.4.1"
port = 5683
path ="coap"

client = HelperClient(server=(host, port))
response = client.get(path)
print response.pretty_print()
client.stop()