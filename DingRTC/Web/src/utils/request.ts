export const request = (url: string, params: any) => {
  url = _getUrl(url, params);
  let res: Response;
  return new Promise((resolve, reject) => {
    fetch(url, {
      headers: {
        'Content-Type': 'application/json',
      },
    })
      .then((response) => {
        res = response;
        if (response.ok) {
          return response.json();
        } else {
          return response.text();
        }
      })
      .then((data) => {
        if (res?.ok) {
          if (data.code) {
            reject(data);
          } else {
            resolve(data.data);
          }
        } else {
          reject(data);
        }
      })
      .catch((error) => {
        reject(error);
      });
  });
};

const _getUrl = (url: string, params: any) => {
  if (!params) return url;
  const paramsArr = [];
  for (const key in params) {
    paramsArr.push(key + '=' + params[key]);
  }
  const args = paramsArr.join('&');
  url = (url.indexOf('?') != -1 ? url : url + '?') + args;
  return url;
};
