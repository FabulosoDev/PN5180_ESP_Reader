import { EventHandler } from './common.js';
import { Nfc } from './nfc.js';

$(function() {
    const eventHandler = new EventHandler();
    eventHandler.loadNavbar('navHistory');

    const events = {
        'save_success': e => {
            eventHandler.showMessage('success', e.data);
            loadCardHistory();
            $.isFunction(navigator.vibrate) && navigator.vibrate([200, 100, 200]);
        },
        'save_duplicate': 'light',
        'save_error': 'danger'
    };
    eventHandler.setupEventSource(events);

    function loadCardHistory() {
        const $tbody = $('#cardHistory');

        $.ajax({
            url: '/cards.json',
            method: 'GET',
            cache: false,
            headers: {
                'Cache-Control': 'no-cache',
                'Pragma': 'no-cache'
            }
        })
        .done(cards => {
            $tbody.empty();

            const rows = cards.map(card => {
                const $actions = $('<td>').append(
                    $('<button>')
                        .addClass('btn btn-primary btn-sm me-2')
                        .html('<span class="fa-solid fa-download"></span>')
                        .on('click', () => downloadCard(card)),
                    $('<button>')
                        .addClass('btn btn-danger btn-sm')
                        .html('<span class="fa-solid fa-trash"></span>')
                        .on('click', () => deleteCard(card.uid))
                );

                return $('<tr>').append(
                    $('<td>').text(card.uid),
                    $('<td>').text(card.data.slice(0, 6) + '...'),
                    $actions
                );
            });

            $tbody.append(rows);
        });
    }

    function downloadCard(card) {
        const folderName = card.uid.slice(0, 8).toUpperCase();
        const $modal = $('#zipNameModal');
        const $confirmBtn = $('#confirmZipName');

        $('#zipNameInput').val(folderName);

        const modal = new bootstrap.Modal($modal);
        $modal.one('hidden.bs.modal', () => {
            $confirmBtn.off('click');
        });

        $confirmBtn.one('click', function() {
            modal.hide();
            const zipName = $('#zipNameInput').val().trim() || folderName;

            try {
                const zip = new JSZip();
                const jsonName = card.uid.slice(-8).toUpperCase();
                const folder = zip.folder(folderName);
                const nfc = new Nfc(card.uid, card.data);

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

    function deleteCard(uid) {
        const $modal = $('#deleteConfirmModal');
        const $confirmBtn = $('#confirmDelete');

        const modal = new bootstrap.Modal($modal);
        $modal.one('hidden.bs.modal', () => {
            $confirmBtn.off('click');
        });

        $confirmBtn.one('click', function() {
            modal.hide();
            $.ajax({
                url: '/delete',
                method: 'POST',
                contentType: 'application/json',
                data: JSON.stringify({ uid: uid })
            })
            .done(data => {
                if (data.success) {
                    eventHandler.showMessage('success', 'Card deleted successfully');
                } else {
                    eventHandler.showMessage('danger', data.error || 'Failed to delete card');
                }
            })
            .fail(() => {
                eventHandler.showMessage('danger', 'Failed to delete card');
            })
            .always(() => {
                loadCardHistory();
            });
        });

        modal.show();
    }

    function deleteAll() {
        const $modal = $('#deleteAllConfirmModal');
        const $confirmBtn = $('#confirmDeleteAll');

        const modal = new bootstrap.Modal($modal[0]);
        $modal.one('hidden.bs.modal', () => {
            $confirmBtn.off('click');
        });

        $confirmBtn.one('click', function() {
            modal.hide();
            $.ajax({
                url: '/deleteall',
                method: 'DELETE'
            })
            .done(data => {
                if (data.success) {
                    eventHandler.showMessage('success', 'All cards deleted successfully');
                } else {
                    eventHandler.showMessage('danger', data.error || 'Failed to delete all cards');
                }
            })
            .fail(() => {
                eventHandler.showMessage('danger', 'Failed to delete all cards');
            })
            .always(() => {
                loadCardHistory();
            });
        });

        modal.show();
    }

    $('#btnDeleteAll').on('click', deleteAll);

    loadCardHistory();
});