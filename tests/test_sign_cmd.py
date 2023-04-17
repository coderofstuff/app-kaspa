from application_client.kaspa_transaction import Transaction, TransactionInput, TransactionOutput
from application_client.kaspa_command_sender import KaspaCommandSender, Errors
from application_client.kaspa_response_unpacker import unpack_get_public_key_response, unpack_sign_tx_response
from ragger.backend import RaisePolicy
from ragger.navigator import NavInsID
from utils import ROOT_SCREENSHOT_PATH, check_signature_validity

# In this tests we check the behavior of the device when asked to sign a transaction


# In this test se send to the device a transaction to sign and validate it on screen
# The transaction is short and will be sent in one chunk
# We will ensure that the displayed information is correct by using screenshots comparison
def test_sign_tx_short_tx(firmware, backend, navigator, test_name):
    # Use the app interface instead of raw interface
    client = KaspaCommandSender(backend)
    # The path used for this entire test
    path: str = "m/44'/111111'/0'/0/0"

    # First we need to get the public key of the device in order to build the transaction
    rapdu = client.get_public_key(path=path)
    _, public_key, _, _ = unpack_get_public_key_response(rapdu.data)

    # Create the transaction that will be sent to the device for signing
    transaction = Transaction(
        version=1,
        inputs=[
            TransactionInput(
                value=100000,
                tx_id="e119d53514c1b0e2efce7a89e3d1d5d6cd73582ea20687641c8fdccb6060a9ad",
                address_type=0,
                address_index=0
            )
        ],
        outputs=[
            TransactionOutput(
                value=90000,
                script_public_key="20e9edf67a325868ecc7cd8519e6ca5265e65b7d10f56066461ceabf0c2bc1c5adac"
            )
        ]
    )

    # Send the sign device instruction.
    # As it requires on-screen validation, the function is asynchronous.
    # It will yield the result when the navigation is done
    with client.sign_tx(path=path, transaction=transaction):
        # Validate the on-screen request by performing the navigation appropriate for this device
        if firmware.device.startswith("nano"):
            navigator.navigate_until_text_and_compare(NavInsID.RIGHT_CLICK,
                                                      [NavInsID.BOTH_CLICK],
                                                      "Approve",
                                                      ROOT_SCREENSHOT_PATH,
                                                      test_name)
        else:
            navigator.navigate_until_text_and_compare(NavInsID.USE_CASE_REVIEW_TAP,
                                                      [NavInsID.USE_CASE_REVIEW_CONFIRM,
                                                       NavInsID.USE_CASE_STATUS_DISMISS],
                                                      "Hold to sign",
                                                      ROOT_SCREENSHOT_PATH,
                                                      test_name)

    # The device as yielded the result, parse it and ensure that the signature is correct
    response = client.get_async_response().data
    _, der_sig, _ = unpack_sign_tx_response(response)
    assert check_signature_validity(public_key, der_sig, transaction)


# In this test se send to the device a transaction to sign and validate it on screen
# This test is mostly the same as the previous one but with different values.
# In particular the long memo will force the transaction to be sent in multiple chunks
def test_sign_tx_long_tx(firmware, backend, navigator, test_name):
    # Use the app interface instead of raw interface
    client = KaspaCommandSender(backend)
    path: str = "m/44'/111111'/0'/0/0"

    rapdu = client.get_public_key(path=path)
    _, public_key, _, _ = unpack_get_public_key_response(rapdu.data)

    transaction = Transaction(
        version=1,
        inputs=[
            TransactionInput(
                value=100000,
                tx_id="e119d53514c1b0e2efce7a89e3d1d5d6cd73582ea20687641c8fdccb6060a9ad",
                address_type=0,
                address_index=0
            )
        ],
        outputs=[
            TransactionOutput(
                value=90000,
                script_public_key="20e9edf67a325868ecc7cd8519e6ca5265e65b7d10f56066461ceabf0c2bc1c5adac"
            )
        ]
    )

    with client.sign_tx(path=path, transaction=transaction):
        if firmware.device.startswith("nano"):
            navigator.navigate_until_text_and_compare(NavInsID.RIGHT_CLICK,
                                                      [NavInsID.BOTH_CLICK],
                                                      "Approve",
                                                      ROOT_SCREENSHOT_PATH,
                                                      test_name)
        else:
            navigator.navigate_until_text_and_compare(NavInsID.USE_CASE_REVIEW_TAP,
                                                      [NavInsID.USE_CASE_REVIEW_CONFIRM,
                                                       NavInsID.USE_CASE_STATUS_DISMISS],
                                                      "Hold to sign",
                                                      ROOT_SCREENSHOT_PATH,
                                                      test_name)
    response = client.get_async_response().data
    _, der_sig, _ = unpack_sign_tx_response(response)
    assert check_signature_validity(public_key, der_sig, transaction)


# Transaction signature refused test
# The test will ask for a transaction signature that will be refused on screen
def test_sign_tx_refused(firmware, backend, navigator, test_name):
    # Use the app interface instead of raw interface
    client = KaspaCommandSender(backend)
    path: str = "m/44'/111111'/0'/0/0"

    rapdu = client.get_public_key(path=path)
    _, pub_key, _, _ = unpack_get_public_key_response(rapdu.data)

    transaction = Transaction(
        version=1,
        inputs=[
            TransactionInput(
                value=100000,
                tx_id="e119d53514c1b0e2efce7a89e3d1d5d6cd73582ea20687641c8fdccb6060a9ad",
                address_type=0,
                address_index=0
            )
        ],
        outputs=[
            TransactionOutput(
                value=90000,
                script_public_key="20e9edf67a325868ecc7cd8519e6ca5265e65b7d10f56066461ceabf0c2bc1c5adac"
            )
        ]
    )

    if firmware.device.startswith("nano"):
        with client.sign_tx(path=path, transaction=transaction):
            # Disable raising when trying to unpack an error APDU
            backend.raise_policy = RaisePolicy.RAISE_NOTHING
            navigator.navigate_until_text_and_compare(NavInsID.RIGHT_CLICK,
                                                      [NavInsID.BOTH_CLICK],
                                                      "Reject",
                                                      ROOT_SCREENSHOT_PATH,
                                                      test_name)

        assert client.get_async_response().status == Errors.SW_DENY
    else:
        for i in range(3):
            instructions = [NavInsID.USE_CASE_REVIEW_TAP] * i
            instructions += [NavInsID.USE_CASE_REVIEW_REJECT,
                             NavInsID.USE_CASE_CHOICE_CONFIRM,
                             NavInsID.USE_CASE_STATUS_DISMISS]
            with client.sign_tx(path=path, transaction=transaction):
                backend.raise_policy = RaisePolicy.RAISE_NOTHING
                navigator.navigate_and_compare(ROOT_SCREENSHOT_PATH,
                                               test_name + f"/part{i}",
                                               instructions)
            assert client.get_async_response().status == Errors.SW_DENY
