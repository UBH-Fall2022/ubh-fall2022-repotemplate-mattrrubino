let socket = new WebSocket("wss://changingtheworldwith.tech/user");
var prevScrollpos = window.pageYOffset;
let image;

socket.onopen = function(e) {
//   alert("[open] Connection established");
    image = document.getElementById("image");
};


socket.onmessage = async function(event) {
    const reader = new FileReader();

    reader.addEventListener("loadend", () => {
        const binary = reader.result;
        const encoded = btoa(binary);

        image.src = "data:image/jpg;base64," + encoded;
    });

    reader.readAsBinaryString(event.data);
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


window.onscroll = function() {
    var currentScrollPos = window.pageYOffset;
    if (prevScrollpos > currentScrollPos) {
        document.getElementById("topnav").style.top = "0";
    } 
    else {
        document.getElementById("topnav").style.top = "-50px";
    }
    prevScrollpos = currentScrollPos;
};
