<?php
header('Content-Type: application/json');

// dang nhap vao database
include("config.php");

// Doc gia tri tu database
$sql = "select * from caro_control";
$result = mysqli_query($conn,$sql);

$data = array();
foreach ($result as $row){
    $data[] = $row;
}

mysqli_close($conn);
echo json_encode($data);

?>
