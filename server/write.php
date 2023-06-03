<?php
    // log in vao database
    include("config.php");
    // doc user input
    $row_col = $_POST["row_col"];
    $x_o = $_POST["x_o"];
    $size = $_POST["size"];
    // update lai database
    $sql = "update caro_control set pulse1=0,pulse2=0,row_col=$row_col,x_o=$x_o,size=$size,state=1";
    mysqli_query($conn, $sql);

    mysqli_close($conn);
?>
