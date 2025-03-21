export class EventHandler {
    constructor() {
        this.bs5Utils = new Bs5Utils();
    }

    setupEventSource(events = {}) {
        if (!window.EventSource) return null;

        const $source = $(new EventSource('/events'));
        
        $source
            .on('open', () => console.log('Events Connected'))
            .on('error', e => {
                if (e.target.readyState !== EventSource.OPEN) {
                    console.log('Events Disconnected');
                }
            });

        Object.entries(events).forEach(([event, handler]) => {
            if (typeof handler === 'function') {
                $source.on(event, e => handler(e.originalEvent));
            } else if (typeof handler === 'string') {
                $source.on(event, e => {
                    const data = e.originalEvent instanceof MessageEvent ? e.originalEvent.data : e.originalEvent;
                    this.showMessage(handler, data);
                });
            }
        });

        return $source[0];
    }

    showMessage(type, message) {
        this.bs5Utils.Snack.show(type, message, 3000, true);
    }

    loadNavbar(activeId) {
        $('#common-navbar').load('navbar.html', () => {
            $('.navbar-nav').find(`#${activeId}`).addClass('active');
        });
    }
}