#pragma once
#include "imp_layer.hpp"

namespace horizon {
class ImpPadstack : public ImpLayer {
public:
    ImpPadstack(const std::string &symbol_filename, const std::string &pool_path);

protected:
    void construct() override;

    ActionCatalogItem::Availability get_editor_type_for_action() const
    {
        return ActionCatalogItem::AVAILABLE_IN_PADSTACK;
    };

private:
    void canvas_update() override;
    CorePadstack core_padstack;
};
} // namespace horizon
