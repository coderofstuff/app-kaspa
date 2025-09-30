from application_client.kaspa_command_sender import KaspaCommandSender
from application_client.kaspa_response_unpacker import unpack_get_app_and_version_response
from utils import verify_version


# Test a specific APDU asking BOLOS (and not the app) the name and version of the current app
def test_get_app_and_version(backend):
    # Use the app interface instead of raw interface
    client = KaspaCommandSender(backend)
    # Send the special instruction to BOLOS
    response = client.get_app_and_version()
    # Use an helper to parse the response, assert the values
    app_name, version = unpack_get_app_and_version_response(response.data)

    assert app_name == "Kaspa"
    verify_version(version)
