from application_client.kaspa_command_sender import KaspaCommandSender
from application_client.kaspa_response_unpacker import unpack_get_version_response
from utils import verify_version

# In this test we check the behavior of the device when asked to provide the app version
def test_version(backend):
    # Use the app interface instead of raw interface
    client = KaspaCommandSender(backend)
    # Send the GET_VERSION instruction
    rapdu = client.get_version()
    # Use an helper to parse the response
    parsed_major, parsed_minor, parsed_patch = unpack_get_version_response(rapdu.data)
    verify_version(f"{parsed_major}.{parsed_minor}.{parsed_patch}")
