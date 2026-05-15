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
    !isset($_POST["userID"]) ||
    !isset($_POST["sid"]) ||
    !isset($_POST["mid"]) ||
    !isset($_POST["value"])
) {
    $code = 5;
    goto end;
}

$userID = $_POST["userID"];
$sID    = $_POST["sid"];
$mID    = $_POST["mid"];
$value  = $_POST["value"];

$sql_query = "
INSERT INTO `data` (`userID`, `sID`, `dataValue`, `timestamp`, `mID`)
VALUES ('$userID', '$sID', '$value', NOW(), '$mID')
";

$sql_result = mysqli_query($sql_handle, $sql_query);

if(!$sql_result) {
    echo mysqli_error($sql_handle);
    echo "<br>";
    $code = 3;
    goto end;
}

$code = 0;

end:
echo $code;
?>
