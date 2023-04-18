from io import BytesIO
from typing import Union

from .kaspa_utils import read, read_uint, read_varint, write_varint, UINT64_MAX


class TransactionError(Exception):
    pass

class TransactionInput:
    def __init__(self,
                value: int,
                tx_id: str,
                address_type: int,
                address_index: int):
        self.value: int = value                  # 8 bytes
        self.tx_id: bytes = bytes.fromhex(tx_id) # 32 bytes
        self.address_type: int = address_type    # 1 byte
        self.address_index:int  = address_index  # 4 bytes

    def serialize(self) -> bytes:
        return b"".join([
            self.value.to_bytes(8, byteorder="big"),
            self.tx_id,
            self.address_type.to_bytes(1, byteorder="big"),
            self.address_index.to_bytes(4, byteorder="big")
        ])

    @classmethod
    def from_bytes(cls, hexa: Union[bytes, BytesIO]):
        buf: BytesIO = BytesIO(hexa) if isinstance(hexa, bytes) else hexa

        value: int = read_uint(buf, 8, 'big')
        tx_id: str = read(buf, 32).decode("hex")
        address_type: int = read_uint(buf, 1, 'big')
        address_index: int = read_uint(buf, 4, 'big')

        return cls(value=value, tx_id=tx_id, address_type=address_type, address_index=address_index)

class TransactionOutput:
    def __init__(self,
                 value: int,
                 script_public_key: str):
        self.value = value
        self.script_public_key: bytes = bytes.fromhex(script_public_key)

    def serialize(self) -> bytes:
        return b"".join([
            self.value.to_bytes(8, byteorder="big"),
            self.script_public_key
        ])

    @classmethod
    def from_bytes(cls, hexa: Union[bytes, BytesIO]):
        buf: BytesIO = BytesIO(hexa) if isinstance(hexa, bytes) else hexa

        value: int = read_uint(buf, 8, 'big')
        script_public_key: str = read(buf, 34).decode("hex")

        return cls(value=value, script_public_key=script_public_key)

class Transaction:
    def __init__(self,
                 version: int,
                 inputs: list[TransactionInput],
                 outputs: list[TransactionOutput],
                 do_check: bool = True) -> None:
        self.version: int = version
        self.inputs: list[TransactionInput] = inputs
        self.outputs: list[TransactionOutput] = outputs

        if do_check:
            if not 0 <= self.version <= UINT64_MAX:
                raise TransactionError(f"Bad version: '{self.version}'!")

    def serialize_first_chunk(self) -> bytes:
        return b"".join([
            self.version.to_bytes(2, byteorder="big"),
            len(self.outputs).to_bytes(1, byteorder="big"),
            len(self.inputs).to_bytes(1, byteorder="big")
        ])

    def serialize(self) -> bytes:
        return b"".join([
            self.version.to_bytes(2, byteorder="big"),
            len(self.outputs).to_bytes(1, byteorder="big"),
            len(self.inputs).to_bytes(1, byteorder="big")
        ]).join([
            x.serialize() for x in self.inputs
        ]).join([
            x.serialize() for x in self.outputs
        ])

    # @classmethod
    # def from_bytes(cls, hexa: Union[bytes, BytesIO]):
    #     buf: BytesIO = BytesIO(hexa) if isinstance(hexa, bytes) else hexa

    #     nonce: int = read_uint(buf, 64, byteorder="big")
    #     to: bytes = read(buf, 67)
    #     value: int = read_uint(buf, 64, byteorder="big")
    #     memo_len: int = read_varint(buf)
    #     memo: str = read(buf, memo_len).decode("ascii")

    #     return cls(nonce=nonce, to=to, value=value, memo=memo)
