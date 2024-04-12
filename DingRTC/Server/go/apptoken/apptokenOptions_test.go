package apptoken

import (
	"bytes"
	"testing"
)

func Test_EmptyOptions(t *testing.T) {
	appTokenOptions := CreateAppTokenOptions()
	if appTokenOptions.EngineOptions == nil {
		t.Error("init CreateAppTokenOptions failed")
	}

	optionsBuf, err := appTokenOptions.Pack()
	if err != nil {
		t.Error("init pack failed")
	}

	options, err := UnpackAppTokenOptions(bytes.NewReader(optionsBuf))
	if err != nil || options.EngineOptions == nil || len(options.EngineOptions) != 0 {
		t.Error("init Unpakc failed")
	}
}

func Test_Options(t *testing.T) {
	appTokenOptions := CreateAppTokenOptions()

	appTokenOptions.SetEngineOptions(map[string]string{
		"k1": "v1",
		"k2": "v2",
	})
	optionsBuf, err := appTokenOptions.Pack()
	if err != nil {
		t.Error("init pack failed")
	}

	options, _ := UnpackAppTokenOptions(bytes.NewReader(optionsBuf))
	if err != nil || options.EngineOptions == nil || len(options.EngineOptions) != 2 {
		t.Error("init Unpakc failed")
	}
}
