$(document).ready(function(){
	initCodeTexarea();
	$('div').live( 'pageinit',function(event){initCodeTexarea();});

});
function initCodeTexarea() {
	console.log('initCodeTexarea')
	$("div:jqmData(role='collapsible-set')").bind('expand', function () {
		$('textarea').trigger('keyup');
	}).bind('collapse', function () {
	});
	$('textarea').bind('focus',function(e) {
		this.selectionStart=0; this.selectionEnd=this.value.length;
	});
	$('textarea').bind('touchend',function(e) {
		if($("input,textarea").is(":focus")){
	 		this.selectionStart=0; this.selectionEnd=this.value.length;
		} else {
		}
	});
}
function bindEventTouch(element) {
	element.bind('expand', function () {
		$('textarea').trigger('keyup');
	}).bind('collapse', function () {
	});
}
function selectarea(el) {
	$(el).focus();
}
