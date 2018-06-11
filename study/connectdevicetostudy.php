<?php

$response = array('output' => array());
function addError(&$response, $error) {
    if (!array_key_exists('errors', $response))
        $response += array('errors' => array());
    array_push($response['errors'], $error);
}
if (empty($_REQUEST['devices']))
    addError($response, 'No devices selected');
if (empty($_REQUEST['study']))
    addError($response, 'No study selected');
if (empty($_REQUEST['branches']))
    addError($response, 'No branch selected');
if (array_key_exists('errors', $response)) {
    echo(json_encode($response));
    exit(1);
}

$listStudiesCommand = sprintf(
    'admin-scripts/list-studies.sh'
    . ' --ssh-config "/var/www/apex-study/admin-scripts/ssh-config"');
$listStudiesOutput = array();
$listStudiesExit = 0;
exec($listStudiesCommand, $listStudiesOutput, $listStudiesExit);

array_push($response['output'], $listStudiesCommand);
$response['output'] = array_merge($response['output'], $listStudiesOutput);
array_push($response['output'], '---');

if ($listStudiesExit != 0) {
    addError($response, 'Unable to list studies, report to admin');
    echo(json_encode($response));
    exit(1);
}

if (!in_array($_REQUEST['study'], $listStudiesOutput)) {
    addError($response, 'Selected study ' . $_REQUEST['study'] . ' doesn\'t exist.');
    echo(json_encode($response));
    exit(1);
}

$listDevicesCommand = sprintf(
    'admin-scripts/list-devices.sh'
    . ' --ssh-config "/var/www/apex-study/admin-scripts/ssh-config"');
$listDevicesOutput = array();
$listDevicesExit = 0;
exec($listDevicesCommand, $listDevicesOutput, $listDevicesExit);

array_push($response['output'], $listDevicesCommand);
$response['output'] = array_merge($response['output'], $listDevicesOutput);
array_push($response['output'], '---');

if ($listDevicesExit != 0) {
    addError($response, 'Unable to list devices, report to admin');
    echo(json_encode($response));
    exit(1);
}

foreach ($_REQUEST['devices'] as $device) {
    if (!in_array($device, $listDevicesOutput)) {
        addError($response, 'Selected device ' . $device . ' doesn\'t exist.');
        echo(json_encode($response));
        exit(1);
    }
}

$listBranchesCommand = sprintf('admin-scripts/list-branches.sh'
                                 . ' --ssh-config "/var/www/apex-study/admin-scripts/ssh-config"'
                                 . ' --study %s', escapeshellarg($_REQUEST['study']));
$listBranchesOutput = array();
$listBranchesExit = 0;
exec($listBranchesCommand, $listBranchesOutput, $listBranchesExit);
$branchesParameter = '';
foreach ($_REQUEST['branches'] as $branch) {
    if (!in_array($branch, $listBranchesOutput)) {
        addError($resposne, 'Selected branch ' . $branch . ' doesn\'t exist.');
        echo(json_encode($response));
        exit(1);
    }
}

foreach ($_REQUEST['devices'] as $device) {
    $connectDeviceToStudyCommand = sprintf(
        'admin-scripts/connectdevicetostudy.sh --yes'
        . ' --ssh-config "/var/www/apex-study/admin-scripts/ssh-config"'
        . ' --device %s --study %s 2>&1',
        escapeshellarg($device),
        escapeshellarg($_REQUEST['study']));
    foreach ($_REQUEST['branches'] as $branch)
        $connectDeviceToStudyCommand .= ' --branch ' . escapeshellarg($branch);
    $connectDeviceToStudyOutput = array();
    $connectDeviceToStudyExit = 0;
    exec($connectDeviceToStudyCommand,
         $connectDeviceToStudyOutput,
         $connectDeviceToStudyExit);

    array_push($response['output'], $connectDeviceToStudyCommand);
    $response['output'] = array_merge($response['output'], $connectDeviceToStudyOutput);
    array_push($response['output'], '---');
    if ($connectDeviceToStudyExit != 0)
        addError($response, 'Unable to link device ' . $device
                 . ' to study, see output for details');
}

echo(json_encode($response));

?>
