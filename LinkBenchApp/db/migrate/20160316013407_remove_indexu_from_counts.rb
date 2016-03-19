class RemoveIndexuFromCounts < ActiveRecord::Migration
  def change
	remove_index :counts, [:id1, :link_type]
  end
end
