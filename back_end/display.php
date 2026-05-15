<?php
include "config.php";

$sql_handle = @mysqli_connect($DB_HOSTNAME, $DB_USERNAME, $DB_PASSWORD);
if(!$sql_handle) { $code = 1; goto end; }

$sql_result = @mysqli_select_db($sql_handle, $DB_TO_USE);
if(!$sql_result) { $code = 2; goto end; }

if($_SERVER["REQUEST_METHOD"] != "GET") {
    $code = 4;
    goto end;
}


// =======================
// GET LATEST HEART RATE
// =======================
$sql_heart = "
SELECT
    `user`.`userName`,
    `data`.`dataValue`
FROM `data`
LEFT JOIN `user` ON `data`.`userID` = `user`.`userID`
LEFT JOIN `meas_type` ON `data`.`mID` = `meas_type`.`mID`
WHERE `meas_type`.`measName` = 'Heart Rate'
ORDER BY `data`.`timestamp` DESC
LIMIT 1
";

$result_heart = mysqli_query($sql_handle, $sql_heart);

if(!$result_heart) {
    echo mysqli_error($sql_handle);
    echo "<br>";
    $code = 3;
    goto end;
}

$name = "-";
$heartRate = "-";

if($row = mysqli_fetch_assoc($result_heart)) {
    $name = $row["userName"];
    $heartRate = $row["dataValue"];
}


// =======================
// GET LATEST SPO2
// =======================
$sql_spo2 = "
SELECT
    `user`.`userName`,
    `data`.`dataValue`
FROM `data`
LEFT JOIN `user` ON `data`.`userID` = `user`.`userID`
LEFT JOIN `meas_type` ON `data`.`mID` = `meas_type`.`mID`
WHERE `meas_type`.`measName` = 'SpO2'
ORDER BY `data`.`timestamp` DESC
LIMIT 1
";

$result_spo2 = mysqli_query($sql_handle, $sql_spo2);

if(!$result_spo2) {
    echo mysqli_error($sql_handle);
    echo "<br>";
    $code = 3;
    goto end;
}

$spo2 = "-";

if($row = mysqli_fetch_assoc($result_spo2)) {
    if($name == "-") {
        $name = $row["userName"];
    }

    $spo2 = $row["dataValue"];
}


// =======================
// DISPLAY VALUE ONLY
// =======================
echo "<!DOCTYPE HTML>";
echo "<html>";
echo "<head>";
echo "<title>Heart Rate and SpO2 Values</title>";
echo "</head>";
echo "<body>";

echo "<h2>Latest Reading</h2>";

echo "<p>Name: " . $name . "</p>";
echo "<p>Heart Rate: " . $heartRate . " bpm</p>";
echo "<p>SpO2: " . $spo2 . " %</p>";

echo "</body>";
echo "</html>";

exit;

end:
echo $code;
?>
