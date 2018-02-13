// General utilities with potential uses anywhere

urlParam = function(name) {
    let results = new RegExp('[\?&]' + name + '=([^&#]*)').exec(window.location.href);
    return results[1] || undefined;
};