function init(callback) {
	if ((self.registration) && (self.registration.pushManager) && (self.registration.pushManager.getSubscription)) {
		return self.registration.pushManager.getSubscription().then(function(subscription) {
			if (subscription) {
				var token = "";
				if ((subscription.subscriptionId) && (subscription.endpoint.indexOf(subscription.subscriptionId) < 0))
					token = subscription.endpoint + "/" + subscription.subscriptionId;
				else
					token = subscription.endpoint;
				callback("/@msg.csp?token=" + encodeURIComponent(token));
			}
		});
	}
}

self.addEventListener('activate', function(event) {
    init();
});

function do_notify(url) {
		fetch(url).then(function(response) {
			if (response.status !== 200) {
				console.log('Looks like there was a problem. Status Code: ' + response.status);
				throw new Error();
			}

			return response.json().then(function(data) {
					if (data.error || !data.notification) {
						console.error('The API returned an error.', data.error);
						throw new Error();
					}


					var title = 'Concept Client';
					var body = 'Your attention is needed.';
					var icon = '/@/img/icon-192x192.png';
					var notificationTag = 'concept-client-notification-tag';

					if (data.notification.title)
						title = data.notification.title;
					if (data.notification.message)
						message = data.notification.message;
					if (data.notification.icon)
						icon = data.notification.icon;
					if (data.notification.tag)
						notificationTag = data.notification.tag;
					return self.registration.showNotification(title, {
						body: message,
						icon: icon,
						tag: notificationTag
					});
			});
		}).catch(function(err) {
			console.error('Unable to retrieve data', err);

			var title = 'Concept Client';
			var body = 'Your attention is needed.';
			var icon = '/img/icon-192x192.png';
			var tag = 'concept-client-notification-tag';

			return self.registration.showNotification(title, {  
				body: body,  
				icon: icon,  
				tag: tag  
			});  
		})  
}

self.addEventListener('push', function(event) {  
	event.waitUntil(init(do_notify));
});
