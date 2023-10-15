from application_client.kaspa_command_sender import KaspaCommandSender, Errors
from application_client.kaspa_response_unpacker import unpack_get_public_key_response
from ragger.bip import calculate_public_key_and_chaincode, CurveChoice
from ragger.backend import RaisePolicy
from ragger.navigator import NavInsID, NavIns
from utils import ROOT_SCREENSHOT_PATH


# GET_PUBLIC_KEY works for valid cases in non-confirmation mode
def test_get_public_key_no_confirm_valid(backend):
    for path in ["m/44'/111111'/0'/0/0", "m/44'/111111'/0/0/0", "m/44'/111111'/911'/0/0", "m/44'/111111'/0/1/255", "m/44'/111111'/2147483647/0/0", "m/44'/111111'/0'/0/0", "m/44'/111111'/911'/3/0", "m/44'/111111'"]:
        client = KaspaCommandSender(backend)
        response = client.get_public_key(path=path).data
        _, public_key, _, chain_code = unpack_get_public_key_response(response)

        ref_public_key, ref_chain_code = calculate_public_key_and_chaincode(CurveChoice.Secp256k1, path=path)
        assert public_key.hex() == ref_public_key
        assert chain_code.hex() == ref_chain_code

# GET_PUBLIC_KEY errors for invalid cases in non-confirmation mode
def test_get_public_key_no_confirm_invalid(backend):
    backend.raise_policy = RaisePolicy.RAISE_NOTHING

    for test_case in [
        ("m/33'/0'/0'/0/0", Errors.SW_WRONG_BIP32_PURPOSE),
        ("m/44'/0'/0/0/0", Errors.SW_WRONG_BIP32_COIN_TYPE),
        ("m/44'", Errors.SW_WRONG_BIP32_PATH_LEN),
        ("m/44'/111111'/2147483647/0/0/0", Errors.SW_WRONG_BIP32_PATH_LEN)
    ]:
        client = KaspaCommandSender(backend)
        
        assert client.get_public_key(path=test_case[0]).status == test_case[1]


# In this test we check that the GET_PUBLIC_KEY works in confirmation mode
def test_get_public_key_confirm_accepted(firmware, backend, navigator, test_name):
    client = KaspaCommandSender(backend)
    path = "m/44'/111111'/0'/0/0"
    with client.get_public_key_with_confirmation(path=path):
        if firmware.device.startswith("nano"):
            navigator.navigate_until_text_and_compare(NavInsID.RIGHT_CLICK,
                                                      [NavInsID.BOTH_CLICK],
                                                      "Approve",
                                                      ROOT_SCREENSHOT_PATH,
                                                      test_name)
        else:
            instructions = [
                NavInsID.USE_CASE_REVIEW_TAP,
                NavIns(NavInsID.TOUCH, (200, 335)),
                NavInsID.USE_CASE_ADDRESS_CONFIRMATION_EXIT_QR,
                NavInsID.USE_CASE_ADDRESS_CONFIRMATION_TAP,
                NavInsID.USE_CASE_ADDRESS_CONFIRMATION_CONFIRM,
                NavInsID.USE_CASE_STATUS_DISMISS
            ]
            navigator.navigate_and_compare(ROOT_SCREENSHOT_PATH,
                                           test_name,
                                           instructions)
    response = client.get_async_response().data
    _, public_key, _, chain_code = unpack_get_public_key_response(response)

    ref_public_key, ref_chain_code = calculate_public_key_and_chaincode(CurveChoice.Secp256k1, path=path)
    assert public_key.hex() == ref_public_key
    assert chain_code.hex() == ref_chain_code


# In this test we check that the GET_PUBLIC_KEY in confirmation mode replies an error if the user refuses
def test_get_public_key_confirm_refused(firmware, backend, navigator, test_name):
    client = KaspaCommandSender(backend)
    path = "m/44'/111111'/0'/0/0"

    if firmware.device.startswith("nano"):
        with client.get_public_key_with_confirmation(path=path):
            # Disable raising when trying to unpack an error APDU
            backend.raise_policy = RaisePolicy.RAISE_NOTHING
            navigator.navigate_until_text_and_compare(NavInsID.RIGHT_CLICK,
                                                      [NavInsID.BOTH_CLICK],
                                                      "Reject",
                                                      ROOT_SCREENSHOT_PATH,
                                                      test_name)

        response = client.get_async_response()

        # Assert that we have received a refusal
        assert response.status == Errors.SW_DENY
        assert len(response.data) == 0
    else:
        instructions_set = [
            [
                NavInsID.USE_CASE_REVIEW_REJECT,
                NavInsID.USE_CASE_STATUS_DISMISS
            ],
            [
                NavInsID.USE_CASE_REVIEW_TAP,
                NavInsID.USE_CASE_ADDRESS_CONFIRMATION_CANCEL,
                NavInsID.USE_CASE_STATUS_DISMISS
            ]
        ]
        for i, instructions in enumerate(instructions_set):
            with client.get_public_key_with_confirmation(path=path):
                backend.raise_policy = RaisePolicy.RAISE_NOTHING
                navigator.navigate_and_compare(ROOT_SCREENSHOT_PATH,
                                               test_name + f"/part{i}",
                                               instructions)
            response = client.get_async_response()

            # Assert that we have received a refusal
            assert response.status == Errors.SW_DENY
            assert len(response.data) == 0
