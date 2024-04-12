package apptoken

import (
	"bytes"
	"encoding/binary"
	"errors"
	"io"
	"sort"
)

type AppTokenOptions struct {
	EngineOptions map[string]string
}

func CreateAppTokenOptions() AppTokenOptions {
	return AppTokenOptions{
		EngineOptions: make(map[string]string),
	}
}

func (options *AppTokenOptions) SetEngineOptions(engineOptions map[string]string) {
	options.EngineOptions = engineOptions
}

func (options *AppTokenOptions) Pack() ([]byte, error) {
	buf := new(bytes.Buffer)

	// hasEngineOptions
	hasEngineOptions := options.EngineOptions != nil
	if err := binary.Write(buf, binary.BigEndian, hasEngineOptions); err != nil {
		return nil, err
	}
	if hasEngineOptions {
		if err := binary.Write(buf, binary.BigEndian, int32(len(options.EngineOptions))); err != nil {
			return nil, err
		}

		if len(options.EngineOptions) > 0 {
			// sort by key
			keys := make([]string, 0, len(options.EngineOptions))
			for k := range options.EngineOptions {
				keys = append(keys, k)
			}
			sort.Strings(keys)

			for _, key := range keys {
				value := options.EngineOptions[key]
				if key == "" || value == "" {
					return nil, errors.New("illegal engineOptions entry")
				}
				if err := binary.Write(buf, binary.BigEndian, int32(len(key))); err != nil {
					return nil, err
				}
				if _, err := buf.Write([]byte(key)); err != nil {
					return nil, err
				}

				if err := binary.Write(buf, binary.BigEndian, int32(len(value))); err != nil {
					return nil, err
				}
				if _, err := buf.Write([]byte(value)); err != nil {
					return nil, err
				}
			}
		}
	}

	return buf.Bytes(), nil
}

func UnpackAppTokenOptions(buf io.Reader) (*AppTokenOptions, error) {
	options := &AppTokenOptions{
		EngineOptions: make(map[string]string),
	}
	// hasEngineOptions
	var hasEngineOptions bool
	if err := binary.Read(buf, binary.BigEndian, &hasEngineOptions); err != nil {
		return nil, err
	}

	if hasEngineOptions {

		var size int32
		if err := binary.Read(buf, binary.BigEndian, &size); err != nil {
			return nil, err
		}

		for i := int32(0); i < size; i++ {
			var keyLength int32
			if err := binary.Read(buf, binary.BigEndian, &keyLength); err != nil {
				return nil, err
			}
			key := make([]byte, keyLength)
			if _, err := buf.Read(key); err != nil {
				return nil, err
			}

			var valueLength int32
			if err := binary.Read(buf, binary.BigEndian, &valueLength); err != nil {
				return nil, err
			}
			value := make([]byte, valueLength)
			if _, err := buf.Read(value); err != nil {
				return nil, err
			}

			options.EngineOptions[string(key)] = string(value)
		}
	}

	return options, nil
}
