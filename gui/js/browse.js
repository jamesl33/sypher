function emptyUpload() {
  if (document.getElementById("upload").value == "") {
    document.getElementById("next").style.backgroundColor = "#c0c0c0";
    document.getElementById("link").href = "";
  } else {
    document.getElementById("next").style.backgroundColor = "#535ae8";
    document.getElementById("link").href = "./encode_download.html";
  }
  window.setTimeout("emptyUpload();", 1000 * 0.1);
}

function previewImage(input) {
  var reader = new FileReader();
  reader.onload = function() {
    var output = document.getElementById("preview");
    document.getElementById("preview").style.boxShadow =
      "0px 0px 35px -1px rgba(0,122,255,0.7)";
    output.src = reader.result;
  };
  reader.readAsDataURL(event.target.files[0]);
}
