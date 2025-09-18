import { EventHandler } from './common.js';

$(function() {
    const eventHandler = new EventHandler();
    eventHandler.loadNavbar('navSettings');

    const events = {
        'settings_success': 'success',
        'settings_warning': 'warning',
        'settings_error': 'danger',
        'restart': 'info'
    };
    eventHandler.setupEventSource(events);

    $.getJSON("/settings", json => {
        Object.entries(json).forEach(([name, value]) => {
            $(`input[name='${name}']`).val(value);
        });
    });

    $('#togglePassword').on('click', e => {
        e.preventDefault();
        const $input = $('#inputPassword');
        const $icon = $('#togglePasswordIcon');
        const isPassword = $input.attr('type') === 'password';

        $input.attr('type', isPassword ? 'text' : 'password');
        $icon.toggleClass('fa-eye fa-eye-slash');
    });

    $('#toggleToken').on('click', e => {
        e.preventDefault();
        const $input = $('#inputToken');
        const $icon = $('#toggleTokenIcon');
        const isPassword = $input.attr('type') === 'password';

        $input.attr('type', isPassword ? 'text' : 'password');
        $icon.toggleClass('fa-eye fa-eye-slash');
    });

    $('#btnRestart').on('click', () => {
        const $button = $('#btnRestart').prop('disabled', true);

        $.ajax({
            type: 'GET',
            url: '/restart'
        })
        .always(() => $button.prop('disabled', false));
    });

    $('#formSettings').on('submit', e => {
        e.preventDefault();
        const $form = $(e.target);
        const $button = $('#btnSubmit').prop('disabled', true);

        $.ajax({
            type: 'POST',
            url: '/settings',
            data: JSON.stringify(Object.fromEntries(new FormData($form[0]))),
            contentType: 'application/json',
            processData: false
        })
        .always(() => $button.prop('disabled', false));
    });
});