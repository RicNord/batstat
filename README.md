# Batstat

Battery notifications.

## Dependencies

- libnotify

## Installation

```bash
sudo make install
```

### Start systemd service

```bash
systemctl --user daemon-reload
systemctl --user enable --now batstat
```
