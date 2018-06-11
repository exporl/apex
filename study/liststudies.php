<?php
echo(json_encode(
    array_filter(explode("\n", shell_exec(
        'admin-scripts/list-studies.sh'
        . ' --ssh-config "/var/www/apex-study/admin-scripts/ssh-config"')))));
?>
