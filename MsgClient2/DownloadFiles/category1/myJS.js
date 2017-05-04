
$(document).ready(function(){
$(".header").click(function () {
    $header = $(this);
    $content = $header.next();
    $content.slideToggle(300, function () {
        $header.text(function () {
            return $content.is(":visible") ? "-  {" : "+  {";
        });
    });
});
});
/*
$("#button").click(function(){
    if($(this).html() == "-"){
        $(this).html("+");
        console.log("1");
    }
    else{
        $(this).html("-");
        console.log("2");
    }
    $("#box").slideToggle();
});*/