#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <unistd.h>
#include <libnotify/notification.h>
#include <libnotify/notify.h>

#define POWER_SUPPLY_CAPACITY "/sys/class/power_supply/BAT0/capacity"
#define POWER_SUPPLY_STATUS "/sys/class/power_supply/BAT0/status"

int battery_perc(char **buffer, size_t n)
{
	static char *path = POWER_SUPPLY_CAPACITY;
	FILE *fp;
	if ((fp = fopen(path, "r")) != NULL) {
		getline(buffer, &n, fp);
	}
	return fclose(fp);
}

int battery_state(char **buffer, size_t n)
{
	static char *path = POWER_SUPPLY_STATUS;
	FILE *fp;
	if ((fp = fopen(path, "r")) != NULL) {
		getline(buffer, &n, fp);
	}
	return fclose(fp);
}

void send_notification(const char *msg, int is_critical)
{
	notify_init("batstat");
	NotifyNotification *Notification =
		notify_notification_new("batstat", msg, NULL);
	if (is_critical) {
		notify_notification_set_timeout(Notification,
						NOTIFY_EXPIRES_NEVER);
		notify_notification_set_urgency(Notification,
						NOTIFY_URGENCY_CRITICAL);
	}
	notify_notification_show(Notification, NULL);
	g_object_unref(G_OBJECT(Notification));
	notify_uninit();
}

int main()
{
	int LESS_20_SWITCH = 0;
	int LESS_10_SWITCH = 0;
	int LESS_05_SWITCH = 0;
	char *percent_buffer;
	char *state_buffer;
	size_t percent_bufsize = 4;
	size_t state_bufsize = 20;

	do {
		percent_buffer = (char *)malloc(percent_bufsize * sizeof(char));
		state_buffer = (char *)malloc(state_bufsize * sizeof(char));
		if (percent_buffer == NULL || state_buffer == NULL) {
			perror("Unable to allocate buffers");
		}
		battery_perc(&percent_buffer, percent_bufsize);
		battery_state(&state_buffer, state_bufsize);

		if (strncmp(state_buffer, "Discharging", 11) == 0) {
			if (atoi(percent_buffer) < 05 && LESS_05_SWITCH == 0) {
				send_notification("Battery below 5%", 1);
				LESS_05_SWITCH = 1;
				LESS_10_SWITCH = 1;
				LESS_20_SWITCH = 1;
			} else if (atoi(percent_buffer) < 10 &&
				   LESS_10_SWITCH == 0) {
				send_notification("Battery below 10%", 0);
				LESS_10_SWITCH = 1;
				LESS_20_SWITCH = 1;
			} else if (atoi(percent_buffer) < 20 &&
				   LESS_20_SWITCH == 0) {
				send_notification("Battery below 20%", 0);
				LESS_20_SWITCH = 1;
			};
		}

		if (strncmp(state_buffer, "Discharging", 11) != 0) {
			if (atoi(percent_buffer) >= 20 && LESS_20_SWITCH == 1) {
				LESS_20_SWITCH = 0;
				LESS_10_SWITCH = 0;
				LESS_05_SWITCH = 0;
			} else if (atoi(percent_buffer) >= 10 &&
				   LESS_10_SWITCH == 1) {
				LESS_10_SWITCH = 0;
				LESS_05_SWITCH = 0;
			} else if (atoi(percent_buffer) >= 5 &&
				   LESS_05_SWITCH == 1) {
				LESS_05_SWITCH = 0;
			};
		}
		free(percent_buffer);
		free(state_buffer);
	} while (!sleep(10));

	return 0;
}
