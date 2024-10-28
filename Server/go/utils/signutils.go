package utils

import (
	"crypto/hmac"
	"crypto/sha256"
	"encoding/binary"
)

const (
	AlgorithmHmacSHA256 = "sha256"
	IntLength           = 4
)

func GenerateSign(secretKey string, timestamp int32, salt int32) ([]byte, error) {
	timestampBytes := make([]byte, IntLength)
	binary.BigEndian.PutUint32(timestampBytes, uint32(timestamp))

	hash1 := hmac.New(sha256.New, timestampBytes)
	if _, err := hash1.Write([]byte(secretKey)); err != nil {
		return nil, err
	}

	signingT := hash1.Sum(nil)

	saltBytes := make([]byte, IntLength)
	binary.BigEndian.PutUint32(saltBytes, uint32(salt))

	hash2 := hmac.New(sha256.New, saltBytes)
	if _, err := hash2.Write(signingT); err != nil {
		return nil, err
	}

	signingS := hash2.Sum(nil)
	return signingS, nil

}

func Sign(key []byte, data []byte) ([]byte, error) {
	hash := hmac.New(sha256.New, key)
	if _, err := hash.Write(data); err != nil {
		return nil, err
	}

	return hash.Sum(nil), nil
}
