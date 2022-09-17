#include <engine/frame.hpp>
#include <engine/resources.hpp>
#include <game/attachments/hud.hpp>
#include <game/layout.hpp>
#include <game/theme.hpp>
#include <tardigrade/services.hpp>
#include <vulkify/ttf/ttf.hpp>

namespace cronch {
void Hud::setup() {}

void Hud::tick(tg::DeltaTime) {}

void Hud::render(tg::RenderTarget const& target) const { [[maybe_unused]] auto const& frame = static_cast<Frame const&>(target); }
} // namespace cronch
