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
        'save_error': 'danger'
    };
    eventHandler.setupEventSource(events);

    function readCard() {
        const readButton = $('#btnRead').prop('disabled', true);
        const downloadButton = $('#btnDownload').prop('disabled', true);

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
            })
            .fail(() => {
                currentUid = '-';
                currentData = '-';
                $('#cardUid, #cardData').text('-');
                downloadButton.prop('disabled', true);
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
        const folderName = currentUid.slice(0, 8).toUpperCase();
        const $modal = $('#zipNameModal');
        const $confirmBtn = $('#confirmZipName');

        $('#zipNameInput').val(folderName);

        const modal = new bootstrap.Modal($modal);
        $modal.one('hidden.bs.modal', () => {
            readButton.prop('disabled', false);
            downloadButton.prop('disabled', currentUid === '-');
            $confirmBtn.off('click');
        });

        $confirmBtn.one('click', function() {
            modal.hide();
            const zipName = $('#zipNameInput').val().trim() || folderName;

            try {
                const zip = new JSZip();
                const jsonName = currentUid.slice(-8).toUpperCase();
                const folder = zip.folder(folderName);
                const nfc = new Nfc(currentUid, currentData);

                folder.file(`${jsonName}.json`, nfc.createJsonContent());
                folder.file(`${zipName}.nfc`, nfc.createNfcContent());

                zip.generateAsync({type: "blob"})
                    .then(blob => {
                        const url = window.URL.createObjectURL(blob);
                        const link = document.createElement('a');
                        link.href = url;
                        link.download = `${zipName}.zip`;
                        link.style.display = 'none';

                        document.body.appendChild(link);
                        link.click();

                        document.body.removeChild(link);
                        window.URL.revokeObjectURL(url);
                        eventHandler.showMessage('success', 'Zip downloaded successfully');
                    })
                    .catch(error => {
                        eventHandler.showMessage('danger', `Failed to download: ${error.message}`);
                    });

            } catch (error) {
                eventHandler.showMessage('danger', `Failed to create files: ${error.message}`);
            }
        });

        modal.show();
    }

    $('#btnRead').on('click', readCard);
    $('#btnDownload').on('click', downloadCard).prop('disabled', true);
});
