function emptyTextbox() {
    if (document.getElementById("textbox").value == "") {
        document.getElementById("next").style.backgroundColor = "#c0c0c0" 
        document.getElementById("link").href = ""
    }
    else {
        document.getElementById("next").style.backgroundColor = "#535ae8"
        document.getElementById("link").href = "encode_upload_image.html"
    }   
    window.setTimeout("emptyTextbox();", 1000*0.1);
}
