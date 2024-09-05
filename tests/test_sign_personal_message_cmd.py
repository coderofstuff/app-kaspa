import pytest

from application_client.kaspa_command_sender import KaspaCommandSender, Errors, InsType, P1, P2
from application_client.kaspa_message import PersonalMessage
from application_client.kaspa_response_unpacker import unpack_get_public_key_response, unpack_sign_message_response
from ragger.backend import RaisePolicy
from ragger.error import ExceptionRAPDU
from ragger.navigator import NavInsID
from utils import ROOT_SCREENSHOT_PATH, check_signature_validity

# In this tests we check the behavior of the device when asked to sign a transaction


# In this test se send to the device a transaction to sign and validate it on screen
# We will ensure that the displayed information is correct by using screenshots comparison
def test_sign_message_simple(firmware, backend, scenario_navigator, test_name):
    # Use the app interface instead of raw interface
    client = KaspaCommandSender(backend)
    # The path used for this entire test
    path: str = "m/44'/111111'/0'/1/5"

    # First we need to get the public key of the device in order to build the transaction
    rapdu = client.get_public_key(path=path)
    _, public_key, _, _ = unpack_get_public_key_response(rapdu.data)

    address_type = 1
    address_index = 5
    message = "Hello Kaspa!"

    message_data = PersonalMessage(message, address_type, address_index)

    # Send the sign device instruction.
    # As it requires on-screen validation, the function is asynchronous.
    # It will yield the result when the navigation is done
    with client.sign_message(message_data=message_data):
        # Validate the on-screen request by performing the navigation appropriate for this device
        scenario_navigator.review_approve()

    # The device as yielded the result, parse it and ensure that the signature is correct
    response = client.get_async_response().data
    _, der_sig, _, message_hash = unpack_sign_message_response(response)

    assert message_hash == message_data.to_hash()
    assert check_signature_validity(public_key, der_sig, message_hash)

def test_sign_message_simple_different_account(firmware, backend, scenario_navigator, test_name):
    # Use the app interface instead of raw interface
    client = KaspaCommandSender(backend)
    # The path used for this entire test
    path: str = "m/44'/111111'/1'/1/5"

    # First we need to get the public key of the device in order to build the transaction
    rapdu = client.get_public_key(path=path)
    _, public_key, _, _ = unpack_get_public_key_response(rapdu.data)

    address_type = 1
    address_index = 5
    account = 0x80000001 # This is account 1'
    message = "Hello Kaspa!"

    message_data = PersonalMessage(message, address_type, address_index, account)

    # Send the sign device instruction.
    # As it requires on-screen validation, the function is asynchronous.
    # It will yield the result when the navigation is done
    with client.sign_message(message_data=message_data):
        # Validate the on-screen request by performing the navigation appropriate for this device
        scenario_navigator.review_approve()

    # The device as yielded the result, parse it and ensure that the signature is correct
    response = client.get_async_response().data
    _, der_sig, _, message_hash = unpack_sign_message_response(response)

    assert message_hash == message_data.to_hash()
    assert check_signature_validity(public_key, der_sig, message_hash)

def test_sign_message_kanji(firmware, backend, scenario_navigator, test_name):
    # Use the app interface instead of raw interface
    client = KaspaCommandSender(backend)
    # The path used for this entire test
    path: str = "m/44'/111111'/0'/1/3"

    # First we need to get the public key of the device in order to build the transaction
    rapdu = client.get_public_key(path=path)
    _, public_key, _, _ = unpack_get_public_key_response(rapdu.data)

    address_type = 1
    address_index = 3
    message = "こんにちは世界"

    message_data = PersonalMessage(message, address_type, address_index)

    # Send the sign device instruction.
    # As it requires on-screen validation, the function is asynchronous.
    # It will yield the result when the navigation is done
    with client.sign_message(message_data=message_data):
        # Validate the on-screen request by performing the navigation appropriate for this device
        scenario_navigator.review_approve()

    # The device as yielded the result, parse it and ensure that the signature is correct
    response = client.get_async_response().data
    _, der_sig, _, message_hash = unpack_sign_message_response(response)

    assert message_hash == message_data.to_hash()
    assert check_signature_validity(public_key, der_sig, message_hash)

def test_sign_message_too_long(firmware, backend, navigator, test_name):
    backend.raise_policy = RaisePolicy.RAISE_NOTHING
    # Use the app interface instead of raw interface
    client = KaspaCommandSender(backend)

    address_type = 1
    address_index = 4
    message = '''Lorem ipsum dolor sit amet. Aut omnis amet id voluptatem eligendi sit accusantium dolorem 33 corrupti necessitatibus hic consequatur quod et maiores alias non molestias suscipit? Est voluptatem magni qui odit eius est eveniet cupiditate id eius'''

    message_data = PersonalMessage(message, address_type, address_index)

    last_response = client.send_raw_apdu(InsType.SIGN_MESSAGE, p1=P1.P1_INPUTS, p2=P2.P2_LAST, data=message_data.serialize())

    assert last_response.status == Errors.SW_MESSAGE_TOO_LONG

def test_sign_message_refused(firmware, backend, scenario_navigator, test_name):
    # Use the app interface instead of raw interface
    client = KaspaCommandSender(backend)

    address_type = 1
    address_index = 6
    message = "Hello Kaspa!"

    message_data = PersonalMessage(message, address_type, address_index)

    with pytest.raises(ExceptionRAPDU) as e:
        with client.sign_message(message_data=message_data):
            scenario_navigator.review_reject()

    # Assert that we have received a refusal
    assert e.value.status == Errors.SW_DENY
    assert len(e.value.data) == 0