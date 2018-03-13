// General utilities with potential uses anywhere

urlParam = function(name) {
    let results = new RegExp('[\?&]' + name + '=([^&#]*)').exec(window.location.href);
    return results[1] || undefined;
};

integerToBytes = function (num) {
    let bytes = [];
    bytes.push((num & 0xFF000000) >> 24);
    bytes.push((num & 0x00FF0000) >> 16);
    bytes.push((num & 0x0000FF00) >>  8);
    bytes.push((num & 0x000000FF));

    return bytes;
};

Object.prototype.splitWithTail = function (delim,count) {
    let parts = this.split(delim);
    let tail = parts.slice(count).join(delim);
    let result = parts.slice(0,count);
    result.push(tail);
    return result;
};