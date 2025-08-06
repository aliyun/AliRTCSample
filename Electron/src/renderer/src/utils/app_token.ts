import configJson from '~/config.json'
import { ByteBuffer } from './bytes_utils'

const VERSION_0 = "000";

const token = configJson.token || '';
const appKey = configJson.appKey || '';

// It is strongly recommended to get app token from app server.
// don't expose appKey in your electron app
export const getAppToken = async (
  userId: string,
  channelId: string,
  appId: string
): Promise<string> => {
  if (token) {
    return token;
  }
  let issueTimestamp = Math.floor(Date.now() / 1000);
  let salt = Math.floor(Math.random() * issueTimestamp);
  let timestamp = issueTimestamp + 60 * 60 * 24;

  const signKey = await generateSign(appKey, issueTimestamp, salt);

  let bufBody = new ByteBuffer(0);

  // appId, issueTimestamp, salt and timestamp
  bufBody.writeString(appId);
  bufBody.writeInt32(issueTimestamp);
  bufBody.writeInt32(salt);
  bufBody.writeInt32(timestamp);

  // channelId, userId and privilege
  let buf = new ByteBuffer(0);
  buf.writeString(channelId);
  buf.writeString(userId);
  let has_privilege = true;
  buf.writeBool(has_privilege);
  if (has_privilege) {
    buf.writeInt32(0);
  }
  bufBody.writeBytes(buf.getvalue(true));

  // engine options
  buf = new ByteBuffer(0);
  let has_engine_options = true;
  buf.writeBool(has_engine_options);
  if (has_engine_options) {
    buf.writeInt32(0);
  }
  bufBody.writeBytes(buf.getvalue(true));

  const sign = await HmacSHA256(signKey, bufBody.getvalue());
  const signature = new Uint8Array(sign);
  buf = new ByteBuffer(0)
  buf.writeInt32(signature.length)
  buf.writeBytes(signature);
  buf.writeBytes(bufBody.getvalue())

  const cdata = await compress(buf.getvalue(), 'deflate');
  const str = btoa(String.fromCharCode(...new Uint8Array(cdata)));

  return VERSION_0 + str;
};

const generateSign = async (key: string, timestamp: number, salt: number) => {
  const bufTs = Buffer.alloc(4);
  bufTs.writeUInt32BE(timestamp, 0);
  const sign1 = await HmacSHA256(bufTs, key);

  const bufSalt = Buffer.alloc(4);
  bufSalt.writeUInt32BE(salt, 0);
  const sign2 = await HmacSHA256(bufSalt, sign1);
  return sign2;
}

const HmacSHA256 = async (key: string | ArrayBuffer | Buffer, data: string | ArrayBuffer | Buffer) => {
  let encodedKey;
  if (typeof key === 'string') {
    encodedKey = new TextEncoder().encode(key);
  } else {
    encodedKey = key;
  }
  let encodedData;
  if (typeof data === 'string') {
    encodedData = new TextEncoder().encode(data);
  } else {
    encodedData = data;
  }
  let cryptoKey = await window.crypto.subtle.importKey(
      'raw', 
      encodedKey, 
      { name: 'HMAC', hash: { name: 'SHA-256' } }, 
      false, 
      ['sign']
  );
  const sign = await window.crypto.subtle.sign(
      'HMAC', 
      cryptoKey, 
      encodedData
  );
  return sign;
};

const compress = async (
  buf: Buffer,
  encoding = 'gzip' as CompressionFormat
): Promise<ArrayBuffer> => {
  const cs = new CompressionStream(encoding)
  const writer = cs.writable.getWriter()
  writer.write(buf)
  writer.close()
  return new Response(cs.readable).arrayBuffer()
}