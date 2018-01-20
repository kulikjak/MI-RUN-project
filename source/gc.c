

void markContext(Context* ctx) {
	int32_t i;

	for (i = 0; i < ctx->count; i++) {

	} // TODO
}

void sweep() {
	// TODO
}

void clearMarks(varList* vars) {
	int32_t i;

	for (i = 0; i < vars->count; i++) {

	} // TODO
}

void GarbageCollect(Context* root, varList* vars) {
	clearMarks(vars);
	markContext(root);
	sweep(vars);
}
