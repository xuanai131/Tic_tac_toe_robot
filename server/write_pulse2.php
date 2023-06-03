<?php
    // log in vao database
    include("config.php");
    // doc user input
    $pulse2 = $_POST["pulse2"];
    // update lai database
    $sql = "update caro_control set pulse1=0,pulse2=$pulse2,row_col=0,size=0,state=2";
    mysqli_query($conn, $sql);

    mysqli_close($conn);
?>
