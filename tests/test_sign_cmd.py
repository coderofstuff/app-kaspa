from application_client.kaspa_transaction import Transaction, TransactionInput, TransactionOutput
from application_client.kaspa_command_sender import KaspaCommandSender, Errors, InsType, P1, P2
from application_client.kaspa_response_unpacker import unpack_get_public_key_response, unpack_sign_tx_response
from ragger.backend import RaisePolicy
from ragger.navigator import NavInsID
from utils import ROOT_SCREENSHOT_PATH, check_signature_validity

# In this tests we check the behavior of the device when asked to sign a transaction


# In this test se send to the device a transaction to sign and validate it on screen
# We will ensure that the displayed information is correct by using screenshots comparison
def test_sign_tx_simple(firmware, backend, navigator, test_name):
    # Use the app interface instead of raw interface
    client = KaspaCommandSender(backend)
    # The path used for this entire test
    path: str = "m/44'/111111'/0'/0/0"

    # First we need to get the public key of the device in order to build the transaction
    rapdu = client.get_public_key(path=path)
    _, public_key, _, _ = unpack_get_public_key_response(rapdu.data)

    # Create the transaction that will be sent to the device for signing
    transaction = Transaction(
        version=0,
        inputs=[
            TransactionInput(
                value=1100000,
                tx_id="40b022362f1a303518e2b49f86f87a317c87b514ca0f3d08ad2e7cf49d08cc70",
                address_type=0,
                address_index=0,
                index=0,
                public_key=public_key[1:33]
            )
        ],
        outputs=[
            TransactionOutput(
                value=1090000,
                script_public_key="2011a7215f668e921013eb7aac9b7e64b9ec6e757c1b648e89388c919f676aa88cac"
            )
        ]
    )

    # Send the sign device instruction.
    # As it requires on-screen validation, the function is asynchronous.
    # It will yield the result when the navigation is done
    with client.sign_tx(transaction=transaction):
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
    _, _, _, der_sig, _, sighash = unpack_sign_tx_response(response)
    assert transaction.get_sighash(0) == sighash
    assert check_signature_validity(public_key, der_sig, sighash, transaction)

def test_sign_tx_simple_change_idx1(firmware, backend, navigator, test_name):
    # Use the app interface instead of raw interface
    client = KaspaCommandSender(backend)
    # The path used for this entire test
    path: str = "m/44'/111111'/0'/0/0"

    # First we need to get the public key of the device in order to build the transaction
    rapdu = client.get_public_key(path=path)
    _, public_key, _, _ = unpack_get_public_key_response(rapdu.data)

    change_path: str = "m/44'/111111'/0'/1/1"

    # First we need to get the public key of the device in order to build the transaction
    change_rapdu = client.get_public_key(path=change_path)
    _, change_public_key, _, _ = unpack_get_public_key_response(change_rapdu.data)

    valid_change_script_public_key = b"".join([
        0x20.to_bytes(1, 'big'),
        change_public_key[1:33],
        0xac.to_bytes(1, 'big')]).hex()

    # Create the transaction that will be sent to the device for signing
    transaction = Transaction(
        version=0,
        change_address_index=1,
        change_address_type=1,
        inputs=[
            TransactionInput(
                value=1100000,
                tx_id="40b022362f1a303518e2b49f86f87a317c87b514ca0f3d08ad2e7cf49d08cc70",
                address_type=0,
                address_index=0,
                index=0,
                public_key=public_key[1:33]
            )
        ],
        outputs=[
            TransactionOutput(
                value=1090000,
                script_public_key=valid_change_script_public_key
            )
        ]
    )

    # Send the sign device instruction.
    # As it requires on-screen validation, the function is asynchronous.
    # It will yield the result when the navigation is done
    with client.sign_tx(transaction=transaction):
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
    _, _, _, der_sig, _, sighash = unpack_sign_tx_response(response)
    assert transaction.get_sighash(0) == sighash
    assert check_signature_validity(public_key, der_sig, sighash, transaction)

def test_sign_tx_with_change(firmware, backend, navigator, test_name):
    # Use the app interface instead of raw interface
    client = KaspaCommandSender(backend)
    # The path used for this entire test
    path: str = "m/44'/111111'/0'/0/0"

    # First we need to get the public key of the device in order to build the transaction
    rapdu = client.get_public_key(path=path)
    _, public_key, _, _ = unpack_get_public_key_response(rapdu.data)

    valid_change_script_public_key = b"".join([
        0x20.to_bytes(1, 'big'),
        public_key[1:33],
        0xac.to_bytes(1, 'big')]).hex()

    # Create the transaction that will be sent to the device for signing
    transaction = Transaction(
        version=0,
        inputs=[
            TransactionInput(
                value=1100000,
                tx_id="40b022362f1a303518e2b49f86f87a317c87b514ca0f3d08ad2e7cf49d08cc70",
                address_type=0,
                address_index=0,
                index=0,
                public_key=public_key[1:33]
            )
        ],
        outputs=[
            TransactionOutput(
                value=1000000,
                script_public_key="2011a7215f668e921013eb7aac9b7e64b9ec6e757c1b648e89388c919f676aa88cac"
            ),
            TransactionOutput(
                value=80000,
                script_public_key=valid_change_script_public_key
            )
        ]
    )

    # Send the sign device instruction.
    # As it requires on-screen validation, the function is asynchronous.
    # It will yield the result when the navigation is done
    with client.sign_tx(transaction=transaction):
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
    _, _, _, der_sig, _, sighash = unpack_sign_tx_response(response)
    assert transaction.get_sighash(0) == sighash
    assert check_signature_validity(public_key, der_sig, sighash, transaction)

def test_sign_tx_with_invalid_change(backend):
    backend.raise_policy = RaisePolicy.RAISE_NOTHING

    client = KaspaCommandSender(backend)
    # The path used for this entire test

    # Create the transaction that will be sent to the device for signing
    tx = Transaction(
        version=0,
        inputs=[
            TransactionInput(
                value=1100000,
                tx_id="40b022362f1a303518e2b49f86f87a317c87b514ca0f3d08ad2e7cf49d08cc70",
                address_type=0,
                address_index=0,
                index=0,
                public_key=None
            )
        ],
        outputs=[
            TransactionOutput(
                value=1090000,
                script_public_key="2011a7215f668e921013eb7aac9b7e64b9ec6e757c1b648e89388c919f676aa88cac"
            ),
            TransactionOutput(
                value=90000,
                script_public_key="200000000000000000000000000000000000000000000000000000000000000000ac"
            )
        ]
    )

    client.send_raw_apdu(InsType.SIGN_TX, p1=P1.P1_START, p2=P2.P2_MORE, data=tx.serialize_first_chunk())
    client.send_raw_apdu(InsType.SIGN_TX, p1=P1.P1_OUTPUTS, p2=P2.P2_MORE, data=tx.outputs[0].serialize())
    client.send_raw_apdu(InsType.SIGN_TX, p1=P1.P1_OUTPUTS, p2=P2.P2_MORE, data=tx.outputs[1].serialize())
    last_response = client.send_raw_apdu(InsType.SIGN_TX, p1=P1.P1_INPUTS, p2=P2.P2_LAST, data=tx.inputs[0].serialize())

    assert last_response.status == Errors.SW_TX_PARSING_FAIL

def test_sign_tx_with_negative_fee(backend):
    backend.raise_policy = RaisePolicy.RAISE_NOTHING

    client = KaspaCommandSender(backend)
    # The path used for this entire test

    # Create the transaction that will be sent to the device for signing
    tx = Transaction(
        version=0,
        inputs=[
            TransactionInput(
                value=1100000,
                tx_id="40b022362f1a303518e2b49f86f87a317c87b514ca0f3d08ad2e7cf49d08cc70",
                address_type=0,
                address_index=0,
                index=0,
                public_key=None
            )
        ],
        outputs=[
            TransactionOutput(
                value=5550000,
                script_public_key="2011a7215f668e921013eb7aac9b7e64b9ec6e757c1b648e89388c919f676aa88cac"
            )
        ]
    )

    client.send_raw_apdu(InsType.SIGN_TX, p1=P1.P1_START, p2=P2.P2_MORE, data=tx.serialize_first_chunk())
    client.send_raw_apdu(InsType.SIGN_TX, p1=P1.P1_OUTPUTS, p2=P2.P2_MORE, data=tx.outputs[0].serialize())
    last_response = client.send_raw_apdu(InsType.SIGN_TX, p1=P1.P1_INPUTS, p2=P2.P2_LAST, data=tx.inputs[0].serialize())

    assert last_response.status == Errors.SW_TX_PARSING_FAIL

def test_sign_tx_invalid_io_len(firmware, backend):
    backend.raise_policy = RaisePolicy.RAISE_NOTHING

    # Use the app interface instead of raw interface
    client = KaspaCommandSender(backend)

    # Outputs must be len 1 or 2 exactly
    # Output is 0xFF
    assert client.send_raw_apdu(
        InsType.SIGN_TX,
        p1=P1.P1_START,
        p2=P2.P2_MORE,
        data=bytes.fromhex("0000FF01")
    ).status == Errors.SW_TX_PARSING_FAIL

    # Output is 3
    assert client.send_raw_apdu(
        InsType.SIGN_TX,
        p1=P1.P1_START,
        p2=P2.P2_MORE,
        data=bytes.fromhex("00000301")
    ).status == Errors.SW_TX_PARSING_FAIL

    # Output is 0
    assert client.send_raw_apdu(
        InsType.SIGN_TX,
        p1=P1.P1_START,
        p2=P2.P2_MORE,
        data=bytes.fromhex("00000001")
    ).status == Errors.SW_TX_PARSING_FAIL

    # Input must be at least 1
    # Input is greater than device supports. 15 for Nano S, 128 for Nano SP and Nano X
    if firmware.device == "nanos":
        assert client.send_raw_apdu(
            InsType.SIGN_TX,
            p1=P1.P1_START,
            p2=P2.P2_MORE,
            data=bytes.fromhex("00000110")
        ).status == Errors.SW_TX_PARSING_FAIL
    else:
        assert client.send_raw_apdu(
            InsType.SIGN_TX,
            p1=P1.P1_START,
            p2=P2.P2_MORE,
            data=bytes.fromhex("00000181")
        ).status == Errors.SW_TX_PARSING_FAIL

    assert client.send_raw_apdu(
        InsType.SIGN_TX,
        p1=P1.P1_START,
        p2=P2.P2_MORE,
        data=bytes.fromhex("00000100")
    ).status == Errors.SW_TX_PARSING_FAIL

def test_sign_tx_inconsistent_input_length_and_data(backend):
    backend.raise_policy = RaisePolicy.RAISE_NOTHING

    # Use the app interface instead of raw interface
    client = KaspaCommandSender(backend)

    tx_input = TransactionInput(
        value=1100000,
        tx_id="40b022362f1a303518e2b49f86f87a317c87b514ca0f3d08ad2e7cf49d08cc70",
        address_type=0,
        address_index=0,
        index=0,
        public_key=None
    )

    # Initialize, setting input length to 1
    client.send_raw_apdu(InsType.SIGN_TX, p1=P1.P1_START, p2=P2.P2_MORE, data=bytes.fromhex("00000101"))

    assert client.send_raw_apdu(
        InsType.SIGN_TX,
        p1=P1.P1_INPUTS,
        p2=P2.P2_MORE,
        data=tx_input.serialize()
    ).status == 0x9000

    # Try sending a second input
    assert client.send_raw_apdu(
        InsType.SIGN_TX,
        p1=P1.P1_INPUTS,
        p2=P2.P2_MORE,
        data=tx_input.serialize()
    ).status == Errors.SW_TX_PARSING_FAIL

def test_sign_tx_inconsistent_output_length_and_data(backend):
    backend.raise_policy = RaisePolicy.RAISE_NOTHING

    # Use the app interface instead of raw interface
    client = KaspaCommandSender(backend)

    output = TransactionOutput(
        value=1,
        script_public_key="2011a7215f668e921013eb7aac9b7e64b9ec6e757c1b648e89388c919f676aa88cac"
    )

    # Initialize, setting output length to 1
    client.send_raw_apdu(InsType.SIGN_TX, p1=P1.P1_START, p2=P2.P2_MORE, data=bytes.fromhex("00000101"))

    assert client.send_raw_apdu(
        InsType.SIGN_TX,
        p1=P1.P1_OUTPUTS,
        p2=P2.P2_MORE,
        data=output.serialize()
    ).status == 0x9000

    # Try sending a second output
    assert client.send_raw_apdu(
        InsType.SIGN_TX,
        p1=P1.P1_OUTPUTS,
        p2=P2.P2_MORE,
        data=output.serialize()
    ).status == Errors.SW_TX_PARSING_FAIL


# In this test se send to the device a transaction to sign and validate it on screen
# This test uses the maximum supported number of inputs per device type
def test_sign_tx_max(firmware, backend, navigator, test_name):
    # Use the app interface instead of raw interface
    client = KaspaCommandSender(backend)
    path: str = "m/44'/111111'/0'/0/0"

    rapdu = client.get_public_key(path=path)
    _, public_key, _, _ = unpack_get_public_key_response(rapdu.data)

    max_input_count = 15 if firmware.device == "nanos" else 128

    inputs = [TransactionInput(
                value=1100000,
                tx_id="40b022362f1a303518e2b49f86f87a317c87b514ca0f3d08ad2e7cf49d08cc" + input_index.to_bytes(1, 'big').hex(),
                address_type=0,
                address_index=0,
                index=0,
                public_key=public_key[1:33]
            ) for input_index in range(max_input_count)]
    output_value = 1090000 * max_input_count

    transaction = Transaction(
        version=0,
        inputs=inputs,
        outputs=[
            TransactionOutput(
                value=output_value,
                script_public_key="2011a7215f668e921013eb7aac9b7e64b9ec6e757c1b648e89388c919f676aa88cac"
            )
        ]
    )

    with client.sign_tx(transaction=transaction):
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
    
    idx = 0
    response = client.get_async_response().data
    has_more, input_index, _, der_sig, _, sighash = unpack_sign_tx_response(response)
    assert transaction.get_sighash(input_index) == sighash
    assert check_signature_validity(public_key, der_sig, sighash, transaction)

    while has_more > 0:
        idx = idx + 1

        if idx > 200:
            break

        response = client.get_next_signature().data
        has_more, input_index, _, der_sig, _, sighash = unpack_sign_tx_response(response)
        assert transaction.get_sighash(input_index) == sighash
        assert check_signature_validity(public_key, der_sig, sighash, transaction)

# Transaction signature refused test
# The test will ask for a transaction signature that will be refused on screen
def test_sign_tx_refused(firmware, backend, navigator, test_name):
    # Use the app interface instead of raw interface
    client = KaspaCommandSender(backend)

    transaction = Transaction(
        version=0,
        inputs=[
            TransactionInput(
                value=1100000,
                tx_id="40b022362f1a303518e2b49f86f87a317c87b514ca0f3d08ad2e7cf49d08cc70",
                address_type=0,
                address_index=0,
                index=0,
                public_key=None
            )
        ],
        outputs=[
            TransactionOutput(
                value=1090000,
                script_public_key="2011a7215f668e921013eb7aac9b7e64b9ec6e757c1b648e89388c919f676aa88cac"
            )
        ]
    )

    if firmware.device.startswith("nano"):
        with client.sign_tx(transaction=transaction):
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
            with client.sign_tx(transaction=transaction):
                backend.raise_policy = RaisePolicy.RAISE_NOTHING
                navigator.navigate_and_compare(ROOT_SCREENSHOT_PATH,
                                               test_name + f"/part{i}",
                                               instructions)
            assert client.get_async_response().status == Errors.SW_DENY
