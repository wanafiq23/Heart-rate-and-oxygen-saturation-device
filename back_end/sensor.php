<?php
include "config.php";

$sql_handle = @mysqli_connect($DB_HOSTNAME, $DB_USERNAME, $DB_PASSWORD);
if(!$sql_handle) { $code = 1; goto end; }

$sql_result = @mysqli_select_db($sql_handle, $DB_TO_USE);
if(!$sql_result) { $code = 2; goto end; }

if($_SERVER["REQUEST_METHOD"] != "POST") {
    $code = 4;
    goto end;
}

if(
    !isset($_POST["sensorName"]) ||
    !isset($_POST["status"]) ||
    !isset($_POST["location"]) ||
    !isset($_POST["macAddress"])
) {
    $code = 5;
    goto end;
}

$sensorName = $_POST["sensorName"];
$status     = $_POST["status"];
$location   = $_POST["location"];
$macAddress = $_POST["macAddress"];

$sql_query = "
INSERT INTO `sensor` (`sensorName`, `status`, `location`, `macAddress`)
VALUES ('$sensorName', '$status', '$location', '$macAddress')
";

$sql_result = mysqli_query($sql_handle, $sql_query);
if(!$sql_result) { $code = 3; goto end; }

$code = 0;

end:
echo $code;
?>
