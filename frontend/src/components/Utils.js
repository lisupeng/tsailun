const HEX_STRINGS = "0123456789abcdef";
const MAP_HEX = {
  0: 0,
  1: 1,
  2: 2,
  3: 3,
  4: 4,
  5: 5,
  6: 6,
  7: 7,
  8: 8,
  9: 9,
  a: 10,
  b: 11,
  c: 12,
  d: 13,
  e: 14,
  f: 15,
  A: 10,
  B: 11,
  C: 12,
  D: 13,
  E: 14,
  F: 15,
};

function searchKey(list, key, curPath) {
  for (var i in list) {
    var node = list[i];
    if (node.key === key) {
      return curPath + "/" + node.pagedir;
    }

    var ret = searchKey(node.children, key, curPath + "/" + node.pagedir);
    if (ret !== "") return ret;
  }

  return "";
}

function KeyPathOfNode(list, key, curPath) {
  for (var i in list) {
    var node = list[i];
    if (node.key === key) {
      return curPath + "/" + node.key;
    }

    var ret = KeyPathOfNode(node.children, key, curPath + "/" + node.key);
    if (ret !== "") return ret;
  }

  return "";
}

const Utils = {
  getNodePathByNodeId(treeData, nodeId) {
    var curpath = "";
    return searchKey(treeData, nodeId, curpath);
  },
  getKeyPathOfNode(treeData, nodeId) {
    var curpath = "";
    return KeyPathOfNode(treeData, nodeId, curpath);
  },
  getDirByNodePath(nodePath) {
    var list = nodePath.split("/");

    if (list.length > 0) return list[list.length - 1];
    else return "";
  },
  getNodeByKey(list, key) {
    for (var i in list) {
      var node = list[i];
      if (node.key === key) {
        return node;
      }

      if (node.children) {
        var ret_node = Utils.getNodeByKey(node.children, key);
        if (ret_node) return ret_node;
      }
    }

    return null;
  },
  getKeyList(list, keylist) {
    for (var i in list) {
      var node = list[i];
      keylist.push(node.key);

      if (node.children) Utils.getKeyList(node.children, keylist);
    }
  },
  replaceNodeData(list, nodedata) {
    for (var i in list) {
      var node = list[i];
      if (node.key === nodedata.key) {
        list[i] = nodedata;
        return;
      }

      if (node.children) {
        Utils.replaceNodeData(node.children, nodedata);
      }
    }
  },

  arrayAminusB(arrA, arrB) {
    var res = [];
    for (var i in arrA) {
      var value = arrA[i];

      let inArrB = false;
      for (var j in arrB) {
        if (arrB[j] === value) inArrB = true;
      }

      if (inArrB === false) res.push(value);
    }

    return res;
  },
  getSpacePathFromUrl(url) {
    var ibegin = url.indexOf("/spaces");
    var iend = url.indexOf("/pages");
    var ipathend = url.indexOf("?");
    var len = url.length;

    if (ibegin !== "-1" && iend !== "-1" && ibegin < iend) {
      return url.substring(ibegin, iend);
    } else if (ibegin !== "-1" && ipathend === "-1") {
      return url.substring(ibegin, len);
    } else if (ibegin !== "-1" && ipathend !== "-1") {
      var space = url.substring(ibegin, ipathend);
      return space;
    } else if (ipathend !== "-1") {
      return url.substring(ibegin, ipathend);
    } else {
      return "/spaces/default";
    }
  },
  getUserDisplayName() {
    var session;
    if (window.localStorage.session) session = window.localStorage.session;
    else if (window.sessionStorage.session)
      session = window.sessionStorage.session;
    else return "guest";

    var sessionObj = JSON.parse(session);
    if (sessionObj.account) {
      return sessionObj.account;
    } else if (sessionObj.email) {
      return sessionObj.email;
    }

    return "guest";
  },
  getSessionId() {
    var session;
    if (window.localStorage.session) session = window.localStorage.session;
    else if (window.sessionStorage.session)
      session = window.sessionStorage.session;
    else return "";

    var sessionObj = JSON.parse(session);
    if (sessionObj.sid) {
      return sessionObj.sid;
    } else {
      return "";
    }
  },
  clearSession() {
    if (window.localStorage.session) window.localStorage.removeItem("session");
    if (window.sessionStorage.session)
      window.sessionStorage.removeItem("session");
  },
  parse(url) {
    return [...new URL(url).searchParams].reduce(
      (cur, [key, value]) => ((cur[key] = value), cur),
      {}
    );
  },
  UtcDateToLocalDate(str) {
    if (!str) return "";

    var list = str.split(" ");

    var yyyy_mm_dd = list[0];
    var hhmmss = list[1];

    var list2 = yyyy_mm_dd.split("-");
    var year = list2[0];
    var mon = list2[1];
    var day = list2[2];

    var list3 = hhmmss.split(":");

    var hour = list3[0];
    var min = list3[1];
    var sec = list3[2];

    var d = new Date();
    d.setUTCFullYear(year, mon - 1, day);
    d.setUTCHours(hour, min, sec);

    var localDate = d.toLocaleString();
    return localDate;
  },

  UtcDateToLocalDate2(str) {
    if (!str) return "";

    var list = str.split(" ");

    var yyyy_mm_dd = list[0];
    var hhmmss = list[1];

    var list2 = yyyy_mm_dd.split("-");
    var year = list2[0];
    var mon = list2[1];
    var day = list2[2];

    var list3 = hhmmss.split(":");

    var hour = list3[0];
    var min = list3[1];
    var sec = list3[2];

    var d = new Date();
    d.setUTCFullYear(year, mon - 1, day);
    d.setUTCHours(hour, min, sec);

    var y = d.getFullYear();
    var _mon =
      d.getMonth() + 1 < 10 ? "0" + (d.getMonth() + 1) : d.getMonth() + 1;
    var _day = d.getDate() < 10 ? "0" + d.getDate() : d.getDate();
    var h = d.getHours() < 10 ? "0" + d.getHours() : d.getHours();
    var m = d.getMinutes() < 10 ? "0" + d.getMinutes() : d.getMinutes();

    return y + "-" + _mon + "-" + _day + " " + h + ":" + m;
  },

  sprint(fmt, args) {
    var result = fmt;
    if (arguments.length > 0) {
      if (arguments.length == 2 && typeof arguments[1] == "object") {
        for (var key in arguments[1]) {
          if (arguments[1][key] != undefined) {
            var reg = new RegExp("({" + key + "})", "g");
            result = result.replace(reg, arguments[1][key]);
          }
        }
      } else {
        for (var i = 1; i < arguments.length; i++) {
          if (arguments[i] != undefined) {
            var reg = new RegExp("({)" + i + "(})", "g");
            result = result.replace(reg, arguments[i]);
          }
        }
      }
    }
    return result;
  },

  logToServer(msg) {
    var url_log2server = "admin/logs?op=writelog";

    fetch(url_log2server, {
      method: "post",
      body: JSON.stringify({ sid: Utils.getSessionId(), log: msg }),
    })
      .then((response) => response.text())
      .then((data) => {
        var res = JSON.parse(data);
        if (res.status === "ok") {
        }
      })
      .catch((error) => console.log("error is", error));
  },

  windowIsNarrow() {
    if (window.innerWidth > 700) return false;
    else return true;
  },

  PathHasSpaceInfo(path) {
    if (path.indexOf("/spaces/") === 0) return true;
    else return false;
  },

  PathHasPageInfo(path) {
    if (path.indexOf("/pages/") === -1) return false;
    else return true;
  },

  getParentPath(path) {
    var idx = path.lastIndexOf("/");
    if (idx === -1) return "";

    return path.substring(0, idx);
  },

  buildUrlForEdit(spacePath, selectedPagePath, newPageDir, loc) {
    if (loc.length === 0 || loc === "child") {
      return (
        spacePath + "/pages" + selectedPagePath + "/" + newPageDir + "?op=edit"
      );
    }

    if (loc === "before" || loc === "after") {
      var selectedPageParentPath = Utils.getParentPath(selectedPagePath);
      return (
        spacePath +
        "/pages" +
        selectedPageParentPath +
        "/" +
        newPageDir +
        "?op=edit"
      );
    }

    if (loc === "root") {
      return spacePath + "/pages" + "/" + newPageDir + "?op=edit";
    }

    return "";
  },

  SHA256(s) {
    var chrsz = 8;

    var hexcase = 0;

    function safe_add(x, y) {
      var lsw = (x & 0xffff) + (y & 0xffff);

      var msw = (x >> 16) + (y >> 16) + (lsw >> 16);

      return (msw << 16) | (lsw & 0xffff);
    }

    function S(X, n) {
      return (X >>> n) | (X << (32 - n));
    }

    function R(X, n) {
      return X >>> n;
    }

    function Ch(x, y, z) {
      return (x & y) ^ (~x & z);
    }

    function Maj(x, y, z) {
      return (x & y) ^ (x & z) ^ (y & z);
    }

    function Sigma0256(x) {
      return S(x, 2) ^ S(x, 13) ^ S(x, 22);
    }

    function Sigma1256(x) {
      return S(x, 6) ^ S(x, 11) ^ S(x, 25);
    }

    function Gamma0256(x) {
      return S(x, 7) ^ S(x, 18) ^ R(x, 3);
    }

    function Gamma1256(x) {
      return S(x, 17) ^ S(x, 19) ^ R(x, 10);
    }

    function core_sha256(m, l) {
      var K = new Array(
        0x428a2f98,
        0x71374491,
        0xb5c0fbcf,
        0xe9b5dba5,
        0x3956c25b,
        0x59f111f1,
        0x923f82a4,
        0xab1c5ed5,
        0xd807aa98,
        0x12835b01,
        0x243185be,
        0x550c7dc3,
        0x72be5d74,
        0x80deb1fe,
        0x9bdc06a7,
        0xc19bf174,
        0xe49b69c1,
        0xefbe4786,
        0xfc19dc6,
        0x240ca1cc,
        0x2de92c6f,
        0x4a7484aa,
        0x5cb0a9dc,
        0x76f988da,
        0x983e5152,
        0xa831c66d,
        0xb00327c8,
        0xbf597fc7,
        0xc6e00bf3,
        0xd5a79147,
        0x6ca6351,
        0x14292967,
        0x27b70a85,
        0x2e1b2138,
        0x4d2c6dfc,
        0x53380d13,
        0x650a7354,
        0x766a0abb,
        0x81c2c92e,
        0x92722c85,
        0xa2bfe8a1,
        0xa81a664b,
        0xc24b8b70,
        0xc76c51a3,
        0xd192e819,
        0xd6990624,
        0xf40e3585,
        0x106aa070,
        0x19a4c116,
        0x1e376c08,
        0x2748774c,
        0x34b0bcb5,
        0x391c0cb3,
        0x4ed8aa4a,
        0x5b9cca4f,
        0x682e6ff3,
        0x748f82ee,
        0x78a5636f,
        0x84c87814,
        0x8cc70208,
        0x90befffa,
        0xa4506ceb,
        0xbef9a3f7,
        0xc67178f2
      );

      var HASH = new Array(
        0x6a09e667,
        0xbb67ae85,
        0x3c6ef372,
        0xa54ff53a,
        0x510e527f,
        0x9b05688c,
        0x1f83d9ab,
        0x5be0cd19
      );

      var W = new Array(64);

      var a, b, c, d, e, f, g, h, i, j;

      var T1, T2;

      m[l >> 5] |= 0x80 << (24 - (l % 32));

      m[(((l + 64) >> 9) << 4) + 15] = l;

      for (var i = 0; i < m.length; i += 16) {
        a = HASH[0];

        b = HASH[1];

        c = HASH[2];

        d = HASH[3];

        e = HASH[4];

        f = HASH[5];

        g = HASH[6];

        h = HASH[7];

        for (var j = 0; j < 64; j++) {
          if (j < 16) W[j] = m[j + i];
          else
            W[j] = safe_add(
              safe_add(
                safe_add(Gamma1256(W[j - 2]), W[j - 7]),
                Gamma0256(W[j - 15])
              ),
              W[j - 16]
            );

          T1 = safe_add(
            safe_add(safe_add(safe_add(h, Sigma1256(e)), Ch(e, f, g)), K[j]),
            W[j]
          );

          T2 = safe_add(Sigma0256(a), Maj(a, b, c));

          h = g;

          g = f;

          f = e;

          e = safe_add(d, T1);

          d = c;

          c = b;

          b = a;

          a = safe_add(T1, T2);
        }

        HASH[0] = safe_add(a, HASH[0]);

        HASH[1] = safe_add(b, HASH[1]);

        HASH[2] = safe_add(c, HASH[2]);

        HASH[3] = safe_add(d, HASH[3]);

        HASH[4] = safe_add(e, HASH[4]);

        HASH[5] = safe_add(f, HASH[5]);

        HASH[6] = safe_add(g, HASH[6]);

        HASH[7] = safe_add(h, HASH[7]);
      }

      return HASH;
    }

    function str2binb(str) {
      var bin = Array();

      var mask = (1 << chrsz) - 1;

      for (var i = 0; i < str.length * chrsz; i += chrsz) {
        bin[i >> 5] |= (str.charCodeAt(i / chrsz) & mask) << (24 - (i % 32));
      }

      return bin;
    }

    function Utf8Encode(string) {
      string = string.replace(/\r\n/g, "\n");

      var utftext = "";

      for (var n = 0; n < string.length; n++) {
        var c = string.charCodeAt(n);

        if (c < 128) {
          utftext += String.fromCharCode(c);
        } else if (c > 127 && c < 2048) {
          utftext += String.fromCharCode((c >> 6) | 192);

          utftext += String.fromCharCode((c & 63) | 128);
        } else {
          utftext += String.fromCharCode((c >> 12) | 224);

          utftext += String.fromCharCode(((c >> 6) & 63) | 128);

          utftext += String.fromCharCode((c & 63) | 128);
        }
      }

      return utftext;
    }

    function binb2hex(binarray) {
      var hex_tab = hexcase ? "0123456789ABCDEF" : "0123456789abcdef";

      var str = "";

      for (var i = 0; i < binarray.length * 4; i++) {
        str +=
          hex_tab.charAt((binarray[i >> 2] >> ((3 - (i % 4)) * 8 + 4)) & 0xf) +
          hex_tab.charAt((binarray[i >> 2] >> ((3 - (i % 4)) * 8)) & 0xf);
      }

      return str;
    }

    s = Utf8Encode(s);

    return binb2hex(core_sha256(str2binb(s), s.length * chrsz));
    //return core_sha256(str2binb(s), s.length * chrsz);
  },

  // Fast Uint8Array to hex
  toHex(bytes) {
    return Array.from(bytes || [])
      .map((b) => HEX_STRINGS[b >> 4] + HEX_STRINGS[b & 15])
      .join("");
  },

  // Mimics Buffer.from(x, 'hex') logic
  // Stops on first non-hex string and returns
  // https://github.com/nodejs/node/blob/v14.18.1/src/string_bytes.cc#L246-L261
  fromHex(hexString) {
    const bytes = new Uint8Array(Math.floor((hexString || "").length / 2));
    let i;
    for (i = 0; i < bytes.length; i++) {
      const a = MAP_HEX[hexString[i * 2]];
      const b = MAP_HEX[hexString[i * 2 + 1]];
      if (a === undefined || b === undefined) {
        break;
      }
      bytes[i] = (a << 4) | b;
    }
    return i === bytes.length ? bytes : bytes.slice(0, i);
  },

  bufferToHex(buffer) {
    var s = "",
      h = "0123456789ABCDEF";
    new Uint8Array(buffer).forEach((v) => {
      s += h[v >> 4] + h[v & 15];
    });
    return s;
  },

  truncateStrForDisplay(str, len) {
    if (str.length <= len) return str;
    else return str.substr(0, len) + "...";
  },

  saveFile(content, fileName) {
    let downLink = document.createElement("a");
    downLink.download = fileName;

    let blob = new Blob([content]);
    downLink.href = URL.createObjectURL(blob);

    document.body.appendChild(downLink);
    downLink.click();

    document.body.removeChild(downLink);
  },
};

export default Utils;
