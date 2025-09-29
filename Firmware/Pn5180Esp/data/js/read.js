import { EventHandler } from './common.js';
import { Nfc } from './nfc.js';

$(function() {
    const eventHandler = new EventHandler();
    eventHandler.loadNavbar('navRead');
    let currentUid = '';
    let currentData = '';

    const events = {
        'unlock_success': 'primary',
        'read_success': e => {
            eventHandler.showMessage('success', e.data);
            readCard();
            $.isFunction(navigator.vibrate) && navigator.vibrate([200, 100, 200]);
        },
        'read_error': 'danger',
        'save_success': 'success',
        'save_duplicate': 'light',
        'save_error': 'danger',
        'discord_queued': 'info',
        'discord_success': 'success',
        'discord_error': 'danger'
    };
    eventHandler.setupEventSource(events);

    function readCard() {
        const readButton = $('#btnRead').prop('disabled', true);
        const downloadButton = $('#btnDownload').prop('disabled', true);
        const nfcDiscordButton = $('#btnNfcDiscord').prop('disabled', true);

        $.getJSON('/read')
            .done(data => {
                currentUid = '-';
                currentData = '-';
                if (data.data && data.uid && !/^0+$/.test(data.uid)) {
                    currentUid = data.uid;
                    currentData = data.data;
                }
                $('#cardUid').text(currentUid);
                $('#cardData').text(currentData);
                downloadButton.prop('disabled', currentUid === '-');
                nfcDiscordButton.prop('disabled', currentUid === '-');
            })
            .fail(() => {
                currentUid = '-';
                currentData = '-';
                $('#cardUid, #cardData').text('-');
                downloadButton.prop('disabled', true);
                nfcDiscordButton.prop('disabled', true);
            })
            .always(() => {
                readButton.prop('disabled', false);
            });
    }

    function downloadCard() {
        if (currentUid === '-' || currentData === '-') {
            eventHandler.showMessage('warning', 'Please read a card first');
            return;
        }

        const readButton = $('#btnRead').prop('disabled', true);
        const downloadButton = $('#btnDownload').prop('disabled', true);
        const nfcName = currentUid.slice(0, 8).toUpperCase();

        try {
            const nfc = new Nfc(currentUid, currentData);
            const content = nfc.createNfcContent();
            const blob = new Blob([content], { type: 'text/plain' });

            const url = window.URL.createObjectURL(blob);
            const link = document.createElement('a');
            link.href = url;
            link.download = `${nfcName}.nfc`;
            link.style.display = 'none';

            document.body.appendChild(link);
            link.click();
            document.body.removeChild(link);
            window.URL.revokeObjectURL(url);

            eventHandler.showMessage('success', 'NFC file downloaded successfully');
        } catch (error) {
            eventHandler.showMessage('danger', `Failed to create NFC file: ${error.message}`);
        } finally {
            readButton.prop('disabled', false);
            downloadButton.prop('disabled', currentUid === '-');
        }
    }

    function nfcDiscord() {
        if (currentUid === '-' || currentData === '-') {
            eventHandler.showMessage('warning', 'Please read a card first');
            return;
        }

        const nfcDiscordButton = $('#btnNfcDiscord').prop('disabled', true);

        $.ajax({
            type: 'POST',
            url: '/nfcdiscord'
        })
        .always(() => {
            nfcDiscordButton.prop('disabled', false);
        });
    }

    $('#btnRead').on('click', readCard);
    $('#btnDownload').on('click', downloadCard).prop('disabled', true);
    $('#btnNfcDiscord').on('click', nfcDiscord).prop('disabled', true);
});
