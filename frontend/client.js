let socket = new WebSocket("wss://changingtheworldwith.tech/user");

socket.onopen = function(e) {
  alert("[open] Connection established");
};

socket.onmessage = function(msg) {
    var arrayBuffer = msg.data;
    var bytes = new Uint8Array(arrayBuffer);

    var image = document.getElementById('image');
    image.src = 'data:image/png;base64,'+ btoa(bytes);
};

socket.onclose = function(event) {
  if (event.wasClean) {
    // alert(`[close] Connection closed cleanly, code=${event.code} reason=${event.reason}`);
  } else {
    // alert('[close] Connection died');
  }
};

socket.onerror = function(error) {
//   alert(`[error]`);
  console.log(error)
};
