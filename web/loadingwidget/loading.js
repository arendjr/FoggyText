/*global define:false, require:false*/
define(["controller", "lib/hogan", "lib/zepto", "text!loadingwidget/loading.html"],
       function(Controller, Hogan, $, loadingHtml) {

    "use strict";

    var loadingEl = null;
    var startTime;

    function init() {

        Controller.addStyle("loadingwidget/loading");

        var template = Hogan.compile(loadingHtml);
        loadingEl = $(template.render()).appendTo(document.body);
    }

    function showLoader() {

        loadingEl.show();

        startTime = (new Date()).getTime();
    }

    function hideLoader() {

        loadingEl.hide();

        var endTime = (new Date()).getTime();
        var totalTime = endTime - startTime;
        console.log("Loading took: " + totalTime + "ms");
    }

    init();

    return {
        "showLoader": showLoader,
        "hideLoader": hideLoader
    };
});
