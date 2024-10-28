package apptoken

import (
	"apptoken/utils"
	"bytes"
	"encoding/base64"
	"encoding/binary"
	"errors"
	"io"
	"time"
)

const VENSION_LENGTH = 3
const BUFFER_CAPACITY_BASE = 256
const VERSION_0 = "000"

type AppToken struct {
	AppId          string
	AppKey         string
	IssueTimestamp int32
	Salt           int32
	Timestamp      int32
	Service        *Service
	Options        *AppTokenOptions
	Signature      []byte
}

func nextMultiple(n, base int) int {
	if base <= 0 || n <= 0 {
		return 0
	}
	result := base
	for result < n {
		result *= 2
	}
	return result
}

func CreateAppToken(appId string, appKey string, timestamp int32) AppToken {

	return AppToken{
		AppId:          appId,
		AppKey:         appKey,
		Salt:           utils.RandomInt32(),
		IssueTimestamp: int32(time.Now().Unix()),
		Timestamp:      timestamp,
	}
}

func (token *AppToken) SetService(service *Service) {
	token.Service = service
}

func (token *AppToken) SetOptions(options *AppTokenOptions) {
	token.Options = options
}

func (token *AppToken) buildSignBody() ([]byte, error) {
	buf := new(bytes.Buffer)
	// appId
	appId := []byte(token.AppId)
	if err := binary.Write(buf, binary.BigEndian, int32(len(appId))); err != nil {
		return nil, errors.New("illegal AppId")
	}
	if _, err := buf.Write(appId); err != nil {
		return nil, errors.New("illegal AppId")
	}

	// issueTimestamp
	if err := binary.Write(buf, binary.BigEndian, token.IssueTimestamp); err != nil {
		return nil, errors.New("illegal IssueTimestamp")
	}

	// salt
	if err := binary.Write(buf, binary.BigEndian, token.Salt); err != nil {
		return nil, errors.New("illegal Salt")
	}

	// timestamp
	if err := binary.Write(buf, binary.BigEndian, token.Timestamp); err != nil {
		return nil, errors.New("illegal Timestamp")
	}

	// service
	service, err := token.Service.Pack()
	if err != nil {
		return nil, errors.New("illegal Service")
	}
	if err := binary.Write(buf, binary.BigEndian, service); err != nil {
		return nil, errors.New("illegal Service")
	}

	// options
	if token.Options == nil {
		token.Options = &AppTokenOptions{
			EngineOptions: make(map[string]string),
		}
	}
	options, err := token.Options.Pack()
	if err != nil {
		return nil, errors.New("illegal AppTokenOptions")
	}
	if err := binary.Write(buf, binary.BigEndian, options); err != nil {
		return nil, errors.New("illegal AppTokenOptions")
	}

	//FIXME
	len := nextMultiple(buf.Len(), BUFFER_CAPACITY_BASE)
	result := make([]byte, len)

	copy(result, buf.Bytes())
	return result, nil
}

func (token *AppToken) Build() (string, error) {
	if token.AppKey == "" {
		return "", errors.New("illegal AppKey")
	}
	if token.Service == nil {
		return "", errors.New("illegal Service")
	}
	token.Service.Validate()

	generatedSign, err := utils.GenerateSign(token.AppKey, token.IssueTimestamp, token.Salt)

	if err != nil {
		return "", errors.New("generate sign failed")
	}

	buf, err := token.buildSignBody()
	if buf == nil || err != nil {
		return "", errors.New("build sign body failed")
	}

	// sign
	sign, err := utils.Sign(generatedSign, buf)

	if err != nil {
		return "", errors.New("sign failed")
	}

	tokenBuf := new(bytes.Buffer)
	// signLength
	if err := binary.Write(tokenBuf, binary.BigEndian, int32(len(sign))); err != nil {
		return "", errors.New("illegal sign")
	}
	// signBody
	if err := binary.Write(tokenBuf, binary.BigEndian, sign); err != nil {
		return "", errors.New("illegal sign")
	}
	// buf
	if err := binary.Write(tokenBuf, binary.BigEndian, buf); err != nil {
		return "", errors.New("illegal buf")
	}

	tokenCompress, err := utils.Compress(tokenBuf.Bytes())
	if err != nil {
		return "", errors.New("token compress failed")
	}

	return VERSION_0 + base64.StdEncoding.EncodeToString(tokenCompress), nil
}

func Parse(tokenStr string) (AppToken, error) {
	appToken := AppToken{}
	if len(tokenStr) <= VENSION_LENGTH || tokenStr[0:VENSION_LENGTH] != VERSION_0 {
		return appToken, errors.New("illegal appToken length")
	}

	tokenOri := tokenStr[VENSION_LENGTH:]
	token, err := base64.StdEncoding.DecodeString(tokenOri)
	if err != nil {
		return appToken, errors.New("base64.decode appToken failed")
	}
	tokenDecompress, err := utils.Decompress(token)
	if err != nil {
		return appToken, errors.New("token decompress failed")
	}

	// sign
	tokenBuf := bytes.NewReader(tokenDecompress)

	// signLegth
	var signLegth int32
	if err := binary.Read(tokenBuf, binary.BigEndian, &signLegth); err != nil {
		return appToken, err
	}
	// signBody
	signature := make([]byte, signLegth)
	if _, err := io.ReadFull(tokenBuf, signature); err != nil {
		return appToken, errors.New("parse sign failed")
	}
	appToken.Signature = signature

	// appId
	var appIdLength int32
	if err := binary.Read(tokenBuf, binary.BigEndian, &appIdLength); err != nil {
		return appToken, err
	}
	appId := make([]byte, appIdLength)
	if _, err := io.ReadFull(tokenBuf, appId); err != nil {
		return appToken, errors.New("parse appId failed")
	}
	appToken.AppId = string(appId)

	// issueTimestamp
	var issueTimestamp int32
	if err := binary.Read(tokenBuf, binary.BigEndian, &issueTimestamp); err != nil {
		return appToken, errors.New("parse issueTimestamp failed")
	}
	appToken.IssueTimestamp = issueTimestamp

	// salt
	var salt int32
	if err := binary.Read(tokenBuf, binary.BigEndian, &salt); err != nil {
		return appToken, errors.New("parse salt failed")
	}
	appToken.Salt = salt

	// timestamp
	var timestamp int32
	if err := binary.Read(tokenBuf, binary.BigEndian, &timestamp); err != nil {
		return appToken, errors.New("parse timestamp failed")
	}
	appToken.Timestamp = timestamp

	// service
	service, err := UnpackService(tokenBuf)
	if err != nil {
		return appToken, errors.New("parse service failed")
	}
	appToken.Service = service

	// options
	options, err := UnpackAppTokenOptions(tokenBuf)
	if err != nil {
		return appToken, errors.New("parse appTokenOptions failed")
	}
	appToken.Options = options

	return appToken, nil
}

func (token *AppToken) Validate(appKey string) bool {
	if appKey == "" {
		return false
	}

	generateSign, err := utils.GenerateSign(appKey, token.IssueTimestamp, token.Salt)

	if err != nil {
		return false
	}

	buf, err := token.buildSignBody()
	if buf == nil || err != nil {
		return false
	}

	// sign
	sign, err := utils.Sign(generateSign, buf)

	if err != nil {
		return false
	}

	return bytes.Equal(sign, token.Signature)
}
