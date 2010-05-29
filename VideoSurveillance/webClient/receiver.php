<?php
/*
*
* Save the submitted file to disk
*/

if (is_uploaded_file($_FILES['img']['tmp_name']))
{
    move_uploaded_file($_FILES['img']['tmp_name'], "img.jpg");
}
?>
<html>
<head>
    <title>Remote Surveillance System</title>
</head>
<body>
    <img name="frame" src="" border="0" />
    <script type="text/javascript">
    function showFrame() {
        document.images.frame.src = "img.jpg?" + Math.random();
        setTimeout("showFrame()", 2000);
    }
    showFrame();
    </script>
</body>
</html>


