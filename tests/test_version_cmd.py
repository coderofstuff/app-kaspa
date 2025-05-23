from application_client.kaspa_command_sender import KaspaCommandSender
from application_client.kaspa_response_unpacker import unpack_get_version_response

# Taken from the Makefile, to update every time the Makefile version is bumped
MAJOR = 1
MINOR = 1
PATCH = 0

# In this test we check the behavior of the device when asked to provide the app version
def test_version(backend):
    # Use the app interface instead of raw interface
    client = KaspaCommandSender(backend)
    # Send the GET_VERSION instruction
    rapdu = client.get_version()
    # Use an helper to parse the response, assert the values
    assert unpack_get_version_response(rapdu.data) == (MAJOR, MINOR, PATCH)
