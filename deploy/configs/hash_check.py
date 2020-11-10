import hashlib
from dirhash import dirhash
import os

def get_hash(file_paths, front_end, back_end):
    hashing = hashlib.new('sha1')

    for filename in file_paths:
        if not os.path.exists(filename):
            continue
        with open(filename,"rb") as f:
            for line in f:
                hashing.update(line)

    frontend_hash = dirhash(front_end, "sha1", ignore=[".*", ".*/"])
    backend_hash = dirhash(back_end, "sha1", ignore=[".*", ".*/"], match=["*.py"])

    hashing.update(frontend_hash.encode())
    hashing.update(backend_hash.encode())
    
    return hashing.hexdigest()

file_paths = [
    '/etc/systemd/system/pufferfish_backend.service',
    '/boot/cmdline.txt',
    '/etc/systemd/system/splashscreen.service',
    '/etc/lightdm/lightdm.conf',
    '/home/pi/.config/lxsession/LXDE-pi/autostart',
    '/home/pi/.config/openbox/lxde-pi-rc.xml',
    '/boot/config.txt',
    '/opt/pufferfish/clean_kiosk.sh',
    '/etc/systemd/system/kiosk.service',
    '/etc/nginx/nginx.conf',
    '/etc/initramfs-tools/modules',
    '/etc/initramfs-tools/scripts/overlay',
    '/usr/local/sbin/overctl',
    '/etc/fstab',
    '/home/pi/.bashrc',
    '/etc/ssh/sshd_config',
    '/etc/fail2ban/jail.local',
    '/etc/modules',
    '/etc/watchdog.conf',
    '/etc/systemd/system/media-pi-LOGS.mount',
    '/etc/rsyslog.conf',
    '/etc/bash.bash_logout',
    '/etc/logrotate.d/pufferfish_logger',
    '/opt/pufferfish/hash_check.py',
    '/opt/pufferfish/compare_hash.sh',
    '/etc/systemd/system/tampering.service',
    '/etc/systemd/system/pufferfish.target'
]

frontend_dir = "/var/www/html"
backend_dir = "pufferfish backend directory path"

backend_service = '/etc/systemd/system/pufferfish_backend.service'

if os.path.exists(backend_service):

    with open(backend_service, 'r') as f:
        file_text = f.read().split('\n')

    for line in file_text:
        if line.startswith('ExecStart='):
            if 'ventserver' not in line.split()[-1]:
                continue
            backend_dir = line.split()[-1]
            backend_dir = backend_dir[:backend_dir.find('ventserver') + 10]
            break

result = get_hash(file_paths, frontend_dir, backend_dir)
print(result)