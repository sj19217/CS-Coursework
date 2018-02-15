// The interactive controls for the speed slider present on each page

document.getElementById("speed_slider").oninput = function () {
    this.title = this.value;
};

// $(document).ready(function(){
//     $('[data-toggle="tooltip"]').tooltip();
// });