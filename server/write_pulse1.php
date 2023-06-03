<?php
    // log in vao database
    include("config.php");
    // doc user input
    $pulse1 = $_POST["pulse1"];
    echo($pulse1);
    // update lai database
    $sql = "update caro_control set pulse1=$pulse1,pulse2=0,row_col=0,size=0,state=2";
    mysqli_query($conn, $sql);

    mysqli_close($conn);
?>
